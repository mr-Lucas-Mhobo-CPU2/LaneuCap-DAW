# LaneuCap DAW

LaneuCap DAW — No Longer a Dream. We Make a Real Make.

This repository contains a C++ JUCE-based DAW scaffold. The initial commit provides a working desktop window, a real audio engine using JUCE's audio device manager and a simple synthesiser so the application produces real audio output.

Build (Windows, using CMake + Visual Studio):

Prerequisites:
- CMake 3.21+
- Visual Studio 2019/2022 (with C++ workload)
- Git

Quick build:

mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release

The CMake script will fetch JUCE automatically during configure. JUCE is used under the GPL by default — if you require a commercial JUCE license, provide it and we will adjust the project accordingly.

License
-------
This repository is licensed MIT by default. Note: JUCE itself is GPL/commercial; bundling JUCE under GPL constrains downstream licensing.
