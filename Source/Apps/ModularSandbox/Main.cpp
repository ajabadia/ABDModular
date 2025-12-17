/*
  ==============================================================================

    ModularSandbox Main.cpp
    Basic host for testing ABDModular modules with GUI.

  ==============================================================================
*/

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "../../UI/Modular/GenericModuleEditor.h"

// Modules
#include "../../Modular/Modules/Oscillators/StandardOscillator/v1/StandardOscillator.h"
#include "../../Modular/Modules/Filters/StandardFilter/v1/StandardFilter.h"
#include "../../Modular/Modules/Effects/StandardReverb/v1/StandardReverb.h"

class MainContentComponent : public juce::AudioAppComponent
{
public:
    MainContentComponent()
    {
        // 1. Create Modules
        osc_ = std::make_unique<Modular::Modules::StandardOscillator>();
        filter_ = std::make_unique<Modular::Modules::StandardFilter>();
        reverb_ = std::make_unique<Modular::Modules::StandardReverb>();

        // 2. Create Editors
        oscEditor_ = std::make_unique<GenericModuleEditor>(osc_.get());
        filterEditor_ = std::make_unique<GenericModuleEditor>(filter_.get());
        reverbEditor_ = std::make_unique<GenericModuleEditor>(reverb_.get());

        addAndMakeVisible(oscEditor_.get());
        addAndMakeVisible(filterEditor_.get());
        addAndMakeVisible(reverbEditor_.get());

        setSize(950, 600);
        setAudioChannels(0, 2);
    }

    ~MainContentComponent() override
    {
        shutdownAudio();
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        osc_->prepareToPlay(sampleRate, samplesPerBlockExpected);
        filter_->prepareToPlay(sampleRate, samplesPerBlockExpected);
        reverb_->prepareToPlay(sampleRate, samplesPerBlockExpected);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto* device = deviceManager.getCurrentAudioDevice();
        auto activeInputChannels = device->getActiveInputChannels();
        auto activeOutputChannels = device->getActiveOutputChannels();
        auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
        auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

        // Process Module Chain
        // Since our modules process frame-by-frame (inefficient but flexible design choice for v1),
        // we loop through the buffer.
        
        // Note: Direct frame-by-frame for buffer size 512 is slow. 
        // But for "Sandboxing", it's acceptable.
        
        auto* left = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* right = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
        
        Modular::Core::AudioFrame frame;
        Modular::Core::AudioFrame outFrame;
        
        for (int i = 0; i < bufferToFill.numSamples; ++i)
        {
            // Oscillator is source
            frame = osc_->processFrame(frame); // Input ignored by osc usually
            
            // Filter
            frame = filter_->processFrame(frame);
            
            // Reverb
            frame = reverb_->processFrame(frame);
            
            // Output
            left[i] = frame.samples[0] * 0.5f; // Master volume safe
            right[i] = frame.samples[1] * 0.5f;
        }
    }

    void releaseResources() override {}

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        oscEditor_->setBounds(area.removeFromLeft(300).reduced(10));
        filterEditor_->setBounds(area.removeFromLeft(300).reduced(10));
        reverbEditor_->setBounds(area.removeFromLeft(300).reduced(10));
    }

private:
    std::unique_ptr<Modular::Modules::StandardOscillator> osc_;
    std::unique_ptr<Modular::Modules::StandardFilter> filter_;
    std::unique_ptr<Modular::Modules::StandardReverb> reverb_;

    std::unique_ptr<GenericModuleEditor> oscEditor_;
    std::unique_ptr<GenericModuleEditor> filterEditor_;
    std::unique_ptr<GenericModuleEditor> reverbEditor_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};

class ModularSandboxApplication : public juce::JUCEApplication
{
public:
    ModularSandboxApplication() {}

    const juce::String getApplicationName() override       { return "ModularSandbox"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise(const juce::String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override {}

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                                 .findColour(juce::ResizableWindow::backgroundColourId),
                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainContentComponent(), true);

            #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
            #else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            #endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(ModularSandboxApplication)
