#include "Transport.h"
#include "../AudioEngine.h"

TransportComponent::TransportComponent()
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(bpmLabel);
    addAndMakeVisible(bpmSlider);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);

    playButton.setButtonText("Play");
    stopButton.setButtonText("Stop");
    saveButton.setButtonText("Save");
    loadButton.setButtonText("Load");

    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmSlider.setRange(40, 240, 1);
    bpmSlider.setValue(AudioEngine::getInstance()->getBpm());

    playButton.onClick = []() { AudioEngine::getInstance()->start(); };
    stopButton.onClick = []() { AudioEngine::getInstance()->stop(); };

    bpmSlider.onValueChange = [this]() { AudioEngine::getInstance()->setBpm(bpmSlider.getValue()); };

    saveButton.onClick = [this]() {
        juce::FileChooser fc("Save project...");
        if (fc.browseForFileToSave(true))
        {
            auto f = fc.getResult();
            AudioEngine::getInstance()->saveProject(f);
        }
    };

    loadButton.onClick = [this]() {
        juce::FileChooser fc("Load project...");
        if (fc.browseForFileToOpen())
        {
            auto f = fc.getResult();
            AudioEngine::getInstance()->loadProject(f);
            bpmSlider.setValue(AudioEngine::getInstance()->getBpm());
        }
    };
}

void TransportComponent::resized()
{
    auto r = getLocalBounds().reduced(6);
    playButton.setBounds(r.removeFromLeft(80));
    stopButton.setBounds(r.removeFromLeft(80));
    saveButton.setBounds(r.removeFromLeft(80));
    loadButton.setBounds(r.removeFromLeft(80));

    bpmLabel.setBounds(r.removeFromLeft(40));
    bpmSlider.setBounds(r.removeFromLeft(120));
}
