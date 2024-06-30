cmake_minimum_required(VERSION 3.29)

project(micfft VERSION 0.0.1 LANGUAGES CXX C)

find_package(Qt6 6.7 COMPONENTS Charts Core Gui Qml Quick REQUIRED)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -stdlib=libc++")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

include(FetchContent)
FetchContent_Declare(
  kissfft
  GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
  GIT_TAG        f5f2a3b2f2cd02bf80639adb12cbeed125bdf420
)
FetchContent_Declare(
  readerwriterqueue
  GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue.git
  GIT_TAG        6b5ed7435be476d93680103a571b486889df0152
)
FetchContent_MakeAvailable(kissfft readerwriterqueue)

qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_add_executable(micfft
    main.cpp
    device.cpp
)

qt_add_qml_module(micfft
    URI andreymlv
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(micfft PRIVATE
    readerwriterqueue
    kissfft
    Qt::Charts
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)
