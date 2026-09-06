#pragma once
#include <JuceHeader.h>

class ChannelRackComponent : public juce::Component
{
public:
    ChannelRackComponent();
    void resized() override;

private:
    struct ChannelControls
    {
        juce::Label name;
        juce::Slider volume;
        juce::ToggleButton mute;
    };

    juce::OwnedArray<juce::Component> channelRows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelRackComponent)
};
