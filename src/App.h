#pragma once
#include <JuceHeader.h>

class LaneuCapApplication : public juce::JUCEApplication
{
public:
    LaneuCapApplication() {}
    const juce::String getApplicationName() override { return "LaneuCap DAW"; }
    const juce::String getApplicationVersion() override { return "0.1"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override;
    void shutdown() override;

private:
    std::unique_ptr<class MainWindow> mainWindow;
};
