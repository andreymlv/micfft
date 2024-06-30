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

#pragma once

#include "common.h"
#include "miniaudio.h"
#include <readerwriterqueue.h>
#include <thread>
#include <QLineSeries>
#include <QObject>

class Device : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QPointF> points MEMBER points NOTIFY pointsChanged)
    Q_PROPERTY(u64 frames MEMBER frames NOTIFY framesChanged)
    Q_PROPERTY(u32 rate MEMBER rate CONSTANT)
private:
    using Queue = moodycamel::ReaderWriterQueue<std::vector<f32>>;

public:
    Device(u32 channels = 1, u32 sample_rate = 44100, QObject *parent = nullptr);
    ~Device() override;

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;

    Device(Device &&other) = delete;
    Device &operator=(Device &&other) = delete;

    bool start();
    bool stop();

signals:
    void pointsChanged();
    void framesChanged();

private:
    static void record_handle(
        ma_device *device, void *output, const void *input, u32 frames
    );

    void reader_handle();

private:
    ma_device_config config = {};
    ma_device *device = nullptr;
    Queue queue;
    std::thread reader;
    std::atomic<bool> started = false;
    QList<QPointF> points = {};
    u64 frames = 0;
    u32 rate = 0;
};
