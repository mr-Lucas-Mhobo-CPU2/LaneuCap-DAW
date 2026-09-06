#pragma once

#include <JuceHeader.h>

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow(juce::String name);
    void closeButtonPressed() override;

private:
    std::unique_ptr<juce::Component> content;
    std::unique_ptr<class TransportComponent> transport;
    std::unique_ptr<class BrowserComponent> browser;
    std::unique_ptr<class PlaylistComponent> playlist;
    std::unique_ptr<class ChannelRackComponent> channelRack;
    std::unique_ptr<class PianoRollComponent> pianoRoll;
    std::unique_ptr<class MixerComponent> mixer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
