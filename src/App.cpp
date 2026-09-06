#include "App.h"
#include "MainWindow.h"

void LaneuCapApplication::initialise(const juce::String& commandLine)
{
    juce::ignoreUnused(commandLine);
    mainWindow = std::make_unique<MainWindow>(getApplicationName());
}

void LaneuCapApplication::shutdown()
{
    mainWindow = nullptr;
}
