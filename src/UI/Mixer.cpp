#include "Mixer.h"
#include "../AudioEngine.h"

MixerComponent::MixerComponent()
{
    refresh();
}

void MixerComponent::refresh()
{
    rows.clear();
    removeAllChildren();

    auto* engine = AudioEngine::getInstance();
    int nch = engine->getNumChannels();

    for (int i = 0; i < nch; ++i)
    {
        auto* row = new juce::Component();
        auto* vol = new juce::Slider();
        vol->setSliderStyle(juce::Slider::LinearVertical);
        vol->setRange(0.0, 1.0, 0.001);
        vol->setValue(engine->getChannelVolume(i));
        row->addAndMakeVisible(vol);

        auto* pan = new juce::Slider();
        pan->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        pan->setRange(-1.0, 1.0, 0.01);
        pan->setValue(engine->getChannelPan(i));
        row->addAndMakeVisible(pan);

        auto* solo = new juce::ToggleButton("S");
        solo->setToggleState(engine->getChannelSolo(i), juce::dontSendNotification);
        row->addAndMakeVisible(solo);

        int idx = i;
        vol->onValueChange = [vol, idx]() { AudioEngine::getInstance()->setChannelVolume(idx, (float)vol->getValue()); };
        pan->onValueChange = [pan, idx]() { AudioEngine::getInstance()->setChannelPan(idx, (float)pan->getValue()); };
        solo->onClick = [solo, idx]() { AudioEngine::getInstance()->setChannelSolo(idx, solo->getToggleState()); };

        rows.add(row);
        addAndMakeVisible(row);
    }
}

void MixerComponent::resized()
{
    auto r = getLocalBounds().reduced(6);
    int colW = 80;
    int x = r.getX();
    for (int i = 0; i < rows.size(); ++i)
    {
        rows[i]->setBounds(x, r.getY(), colW, r.getHeight());
        auto* row = rows[i];
        auto c = row->getLocalBounds();
        if (auto* vol = dynamic_cast<juce::Slider*>(row->getChildComponent(0)))
            vol->setBounds(c.removeFromTop(c.getHeight() - 40).reduced(8));
        if (auto* pan = dynamic_cast<juce::Slider*>(row->getChildComponent(1)))
            pan->setBounds(c.removeFromTop(40).reduced(8));
        if (auto* solo = dynamic_cast<juce::ToggleButton*>(row->getChildComponent(2)))
            solo->setBounds(c.removeFromTop(24).reduced(8).removeFromLeft(24));

        x += colW + 6;
    }
}
