#include "MainWindow.h"
#include "AudioEngine.h"
#include "UI/Browser.h"
#include "UI/Playlist.h"
#include "UI/ChannelRack.h"
#include "UI/PianoRoll.h"
#include "UI/Mixer.h"
#include "UI/Transport.h"

MainWindow::MainWindow(juce::String name)
    : DocumentWindow(name,
                     juce::Colours::lightgrey,
                     DocumentWindow::allButtons)
{
    content = std::make_unique<juce::Component>();

    transport = std::make_unique<TransportComponent>();
    browser = std::make_unique<BrowserComponent>();
    playlist = std::make_unique<PlaylistComponent>();
    channelRack = std::make_unique<ChannelRackComponent>();
    pianoRoll = std::make_unique<PianoRollComponent>();
    mixer = std::make_unique<MixerComponent>();

    content->addAndMakeVisible(transport.get());
    content->addAndMakeVisible(browser.get());
    content->addAndMakeVisible(playlist.get());
    content->addAndMakeVisible(channelRack.get());
    content->addAndMakeVisible(pianoRoll.get());
    content->addAndMakeVisible(mixer.get());

    setContentOwned(content.release(), true);

    centreWithSize(1200, 800);
    setVisible(true);

    // Initialize audio engine
    AudioEngine::getInstance()->initialise();
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
