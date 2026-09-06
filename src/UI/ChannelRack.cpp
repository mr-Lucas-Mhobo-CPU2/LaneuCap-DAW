#include "ChannelRack.h"
#include "../AudioEngine.h"

ChannelRackComponent::ChannelRackComponent()
{
    auto* engine = AudioEngine::getInstance();
    int nch = engine->getNumChannels();

    for (int i = 0; i < nch; ++i)
    {
        auto* row = new juce::Component();
        auto* label = new juce::Label();
        label->setText("Channel " + juce::String(i+1), juce::dontSendNotification);
        row->addAndMakeVisible(label);

        auto* vol = new juce::Slider();
        vol->setRange(0.0, 1.0, 0.01);
        vol->setValue(engine->getChannelVolume(i));
        row->addAndMakeVisible(vol);

        auto* mute = new juce::ToggleButton("Mute");
        mute->setToggleState(engine->getChannelMute(i), juce::dontSendNotification);
        row->addAndMakeVisible(mute);

        int idx = i;
        vol->onValueChange = [vol, idx]() { AudioEngine::getInstance()->setChannelVolume(idx, (float)vol->getValue()); };
        mute->onClick = [mute, idx]() { AudioEngine::getInstance()->setChannelMute(idx, mute->getToggleState()); };

        // store pointers as child components to be laid out later
        channelRows.add(row);
        addAndMakeVisible(row);
    }
}

void ChannelRackComponent::resized()
{
    auto r = getLocalBounds();
    int rowH = 36;
    for (int i = 0; i < channelRows.size(); ++i)
    {
        channelRows[i]->setBounds(0, i * rowH, getWidth(), rowH - 2);
        // layout children of row
        auto* row = channelRows[i];
        auto c = row->getLocalBounds();
        if (auto* label = dynamic_cast<juce::Label*>(row->getChildComponent(0)))
            label->setBounds(c.removeFromLeft(100).reduced(4));
        if (auto* vol = dynamic_cast<juce::Slider*>(row->getChildComponent(1)))
            vol->setBounds(c.removeFromLeft(150).reduced(4));
        if (auto* mute = dynamic_cast<juce::ToggleButton*>(row->getChildComponent(2)))
            mute->setBounds(c.removeFromLeft(80).reduced(6));
    }
}
