#pragma once
#include <JuceHeader.h>

class PlaylistComponent : public juce::Component
{
public:
    PlaylistComponent()
    {
        addAndMakeVisible(label);
        label.setText("Playlist", juce::dontSendNotification);
    }
    void resized() override { label.setBounds(5,5,getWidth()-10,20); }
private:
    juce::Label label;
};
