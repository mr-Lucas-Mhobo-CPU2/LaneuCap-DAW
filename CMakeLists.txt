cmake_minimum_required(VERSION 3.21)
project(LaneuCapDAW VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.2.0
)

FetchContent_MakeAvailable(juce)

juce_add_gui_app(LaneuCapDAW
  PRODUCT_NAME "LaneuCap DAW"
  COMPANY_NAME "LaneuCap"
  IS_MAC_APP FALSE
  SOURCES
    src/main.cpp
    src/App.cpp
    src/MainWindow.cpp
    src/AudioEngine.cpp
    src/UI/Browser.cpp
    src/UI/Playlist.cpp
    src/UI/ChannelRack.cpp
    src/UI/PianoRoll.cpp
    src/UI/Mixer.cpp
    src/UI/Transport.cpp
)

target_compile_features(LaneuCapDAW PRIVATE cxx_std_17)

target_link_libraries(LaneuCapDAW PRIVATE
  juce::juce_audio_utils
  juce::juce_audio_formats
  juce::juce_audio_processors
  juce::juce_gui_basics
)
