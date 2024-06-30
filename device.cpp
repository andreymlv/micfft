// Copyright 2024 Andrey Malov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the “Software”), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "device.h"

#include <iostream>
#include <kissfft.hh>
#include <QPoint>

Device::Device(u32 channels, u32 sample_rate, QObject *parent)
    : QObject(parent)
    , config(ma_device_config_init(ma_device_type_capture))
    , device(new ma_device())
    , queue()
    , rate(sample_rate)
{
    config.capture.format = ma_format_f32;
    config.capture.channels = channels;
    config.sampleRate = rate;
    config.dataCallback = record_handle;
    config.pUserData = &queue;

    if (ma_device_init(nullptr, &config, device) != MA_SUCCESS) {
        std::cerr << "Can't initialize device." << std::endl;
        return;
    }
}

Device::~Device()
{
    stop();
    if (!device) {
        ma_device_uninit(device);
        delete device;
    }
}

bool Device::start()
{
    if (started) {
        std::cerr << "Already started." << std::endl;
        return false;
    }
    if (reader.joinable())
        reader.join();
    started = true;
    if (ma_device_start(device) != MA_SUCCESS) {
        std::cerr << "Can't start device." << std::endl;
        return false;
    }
    reader = std::thread(&Device::reader_handle, this);
    return true;
}

bool Device::stop()
{
    if (ma_device_stop(device) != MA_SUCCESS) {
        std::cerr << "Can't stop device." << std::endl;
        return false;
    }
    started = false;
    if (reader.joinable())
        reader.join();
    return true;
}

void Device::record_handle(
    ma_device *device, void *output, const void *input, u32 frames
)
{
    auto *queue = static_cast<Queue *>(device->pUserData);
    if (!queue)
        return;

    const auto *input_f32 = static_cast<const f32 *>(input);
    const std::vector<f32> in(input_f32, input_f32 + frames);
    if (!queue->try_emplace(std::move(in)))
        return;
}

void Device::reader_handle()
{
    std::vector<f32> in;
    while (started) {
        if (!queue.try_dequeue(in)) {
            continue;
        }
        if (auto f = in.size(); frames != f) {
            frames = f;
            emit framesChanged();
        }

        kissfft<f32> fft(frames, false);
        std::vector<std::complex<f32>> out(frames);
        fft.transform_real(in.data(), out.data());

        QList<QPointF> amplitudes(frames);
        const f32 step = f32(rate) / frames / 2.f;
        for (u64 i = 0; i < frames; ++i) {
            f32 freq = i * step;
            f32 amplitude = abs(out[i]);
            amplitudes[i] = QPointF(freq, amplitude);
        }

        points.swap(amplitudes);
        emit pointsChanged();
    }
}
