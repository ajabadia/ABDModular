// ════════════════════════════════════════════════════════════════════════════
// 1. MOD WHEEL COMPONENT (JUCE)
// ════════════════════════════════════════════════════════════════════════════

#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class ModWheelComponent : public juce::Component,
                         public juce::Slider::Listener
{
public:
    explicit ModWheelComponent()
        : currentValue_(0.0f)
    {
        // Main slider (vertical)
        slider_.setSliderStyle(juce::Slider::LinearVertical);
        slider_.setRange(0.0f, 1.0f, 0.01f);
        slider_.setValue(0.0f);
        slider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider_.addListener(this);
        
        // Visual styling
        slider_.setColour(juce::Slider::trackColourId, 
                         juce::Colour::fromHSV(0.6f, 0.8f, 0.3f, 1.0f));
        slider_.setColour(juce::Slider::thumbColourId, 
                         juce::Colour::fromHSV(0.6f, 0.9f, 0.9f, 1.0f));
        
        addAndMakeVisible(slider_);
        
        setSize(60, 200);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Background circle
        g.setColour(juce::Colours::darkgrey);
        g.fillEllipse(5.0f, 5.0f, 50.0f, 50.0f);
        
        // Border
        g.setColour(juce::Colours::lightgrey);
        g.drawEllipse(5.0f, 5.0f, 50.0f, 50.0f, 2.0f);
        
        // Center dot
        float wheelX = 30.0f;
        float wheelY = 30.0f;
        g.setColour(juce::Colours::white);
        g.fillEllipse(wheelX - 3.0f, wheelY - 3.0f, 6.0f, 6.0f);
        
        // Value display
        g.setColour(juce::Colours::white);
        g.setFont(12.0f);
        g.drawText(juce::String(juce::roundToInt(currentValue_ * 127)), 
                  0, 70, 60, 20, juce::Justification::centred);
        
        // Label
        g.setFont(10.0f);
        g.drawText("MOD WHEEL", 0, 100, 60, 15, juce::Justification::centred);
    }
    
    void resized() override
    {
        slider_.setBounds(10, 130, 40, 70);
    }
    
    void sliderValueChanged(juce::Slider* slider) override
    {
        currentValue_ = (float)slider->getValue();
        repaint();
        
        // Send callback
        if (onModWheelChanged)
            onModWheelChanged(currentValue_);
    }
    
    float getValue() const { return currentValue_; }
    void setValue(float v) 
    { 
        currentValue_ = juce::jlimit(0.0f, 1.0f, v);
        slider_.setValue(currentValue_);
    }
    
    std::function<void(float)> onModWheelChanged;

private:
    juce::Slider slider_;
    float currentValue_;
};


// ════════════════════════════════════════════════════════════════════════════
// 2. PITCH BENDER COMPONENT (JUCE)
// ════════════════════════════════════════════════════════════════════════════

class PitchBenderComponent : public juce::Component,
                            public juce::Slider::Listener
{
public:
    explicit PitchBenderComponent()
        : bendValue_(0.0f)
    {
        // Horizontal slider for pitch bend
        slider_.setSliderStyle(juce::Slider::LinearHorizontal);
        slider_.setRange(-1.0f, 1.0f, 0.01f);
        slider_.setValue(0.0f);
        slider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider_.addListener(this);
        
        slider_.setColour(juce::Slider::trackColourId,
                         juce::Colour::fromHSV(0.0f, 0.8f, 0.3f, 1.0f));
        slider_.setColour(juce::Slider::thumbColourId,
                         juce::Colour::fromHSV(0.0f, 0.9f, 0.9f, 1.0f));
        
        addAndMakeVisible(slider_);
        
        setSize(150, 80);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Background
        g.setColour(juce::Colours::darkgrey);
        g.fillRoundedRectangle(0.0f, 0.0f, (float)getWidth(), (float)getHeight(), 8.0f);
        
        // Border
        g.setColour(juce::Colours::lightgrey);
        g.drawRoundedRectangle(0.0f, 0.0f, (float)getWidth(), (float)getHeight(), 
                              8.0f, 2.0f);
        
        // Center line (zero position)
        float centerX = (float)getWidth() / 2.0f;
        g.setColour(juce::Colours::grey);
        g.drawVerticalLine((int)centerX, 10.0f, (float)(getHeight() - 10));
        
        // Value display
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        
        int cents = juce::roundToInt(bendValue_ * 200.0f);  // ±2 semitones
        juce::String displayText = juce::String(cents) + " cents";
        
        g.drawText(displayText, 0, 5, getWidth(), 20, juce::Justification::centred);
        
        // Label
        g.setFont(10.0f);
        g.drawText("PITCH BEND", 0, getHeight() - 15, getWidth(), 15, 
                  juce::Justification::centred);
    }
    
    void resized() override
    {
        slider_.setBounds(10, 35, getWidth() - 20, 30);
    }
    
    void sliderValueChanged(juce::Slider* slider) override
    {
        bendValue_ = (float)slider->getValue();
        repaint();
        
        if (onPitchBendChanged)
            onPitchBendChanged(bendValue_);
    }
    
    float getValue() const { return bendValue_; }
    void setValue(float v) 
    { 
        bendValue_ = juce::jlimit(-1.0f, 1.0f, v);
        slider_.setValue(bendValue_);
    }
    
    std::function<void(float)> onPitchBendChanged;

private:
    juce::Slider slider_;
    float bendValue_;
};


// ════════════════════════════════════════════════════════════════════════════
// 3. OSCILLOSCOPE COMPONENT (JUCE)
// ════════════════════════════════════════════════════════════════════════════

class OscilloscopeComponent : public juce::Component
{
public:
    static constexpr int WAVEFORM_SIZE = 512;
    
    OscilloscopeComponent()
        : waveformBuffer_(WAVEFORM_SIZE, 0.0f),
          writePos_(0),
          triggerLevel_(0.0f),
          isTriggered_(false)
    {
        setSize(400, 250);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Background (dark grid)
        g.setColour(juce::Colour(20, 20, 30));
        g.fillRect(getLocalBounds());
        
        // Grid lines
        g.setColour(juce::Colour(60, 60, 80));
        
        // Vertical grid
        for (int x = 0; x < getWidth(); x += 50)
        {
            g.drawVerticalLine(x, 0.0f, (float)getHeight());
        }
        
        // Horizontal grid
        for (int y = 0; y < getHeight(); y += 50)
        {
            g.drawHorizontalLine(y, 0.0f, (float)getWidth());
        }
        
        // Center line
        g.setColour(juce::Colour(80, 80, 100));
        g.drawHorizontalLine(getHeight() / 2, 0.0f, (float)getWidth());
        
        // Draw waveform
        drawWaveform(g);
        
        // Info text
        g.setColour(juce::Colours::white);
        g.setFont(11.0f);
        g.drawText("OSCILLOSCOPE", 5, 5, 100, 20, juce::Justification::topLeft);
        
        // RMS/Peak display
        float rms = calculateRMS();
        float peak = calculatePeak();
        
        g.drawText("RMS: " + juce::String(rms, 3), 5, getHeight() - 20, 100, 15, 
                  juce::Justification::topLeft);
        g.drawText("Peak: " + juce::String(peak, 3), 150, getHeight() - 20, 100, 15,
                  juce::Justification::topLeft);
    }
    
    void addSample(float sample)
    {
        // Trigger detection (zero crossing)
        if (!isTriggered_ && sample > triggerLevel_)
        {
            isTriggered_ = true;
            writePos_ = 0;
        }
        
        if (isTriggered_)
        {
            waveformBuffer_[writePos_] = juce::jlimit(-1.0f, 1.0f, sample);
            writePos_ = (writePos_ + 1) % WAVEFORM_SIZE;
        }
        
        repaint();
    }

private:
    std::vector<float> waveformBuffer_;
    int writePos_;
    float triggerLevel_;
    bool isTriggered_;
    
    void drawWaveform(juce::Graphics& g)
    {
        g.setColour(juce::Colour(0, 255, 100));  // Green
        g.setOpaque(false);
        
        juce::Path waveformPath;
        bool firstPoint = true;
        
        float centerY = (float)getHeight() / 2.0f;
        float pixelsPerSample = (float)getWidth() / WAVEFORM_SIZE;
        
        for (int i = 0; i < WAVEFORM_SIZE; ++i)
        {
            float x = i * pixelsPerSample;
            float y = centerY - (waveformBuffer_[i] * centerY * 0.9f);
            
            if (firstPoint)
            {
                waveformPath.startNewSubPath(x, y);
                firstPoint = false;
            }
            else
            {
                waveformPath.lineTo(x, y);
            }
        }
        
        g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
    }
    
    float calculateRMS() const
    {
        float sum = 0.0f;
        for (float sample : waveformBuffer_)
            sum += sample * sample;
        return std::sqrt(sum / waveformBuffer_.size());
    }
    
    float calculatePeak() const
    {
        float peak = 0.0f;
        for (float sample : waveformBuffer_)
            peak = std::max(peak, std::abs(sample));
        return peak;
    }
};


// ════════════════════════════════════════════════════════════════════════════
// 4. KEYBOARD COMPONENT (JUCE)
// ════════════════════════════════════════════════════════════════════════════

class KeyboardComponent : public juce::MidiKeyboardComponent
{
public:
    KeyboardComponent(juce::MidiKeyboardState& keyboardState, 
                     juce::MidiKeyboardComponent::Orientation orientation)
        : juce::MidiKeyboardComponent(keyboardState, orientation)
    {
        // Styling
        setColour(juce::MidiKeyboardComponent::whiteNoteColourId, 
                 juce::Colour(240, 240, 240));
        setColour(juce::MidiKeyboardComponent::blackNoteColourId,
                 juce::Colour(40, 40, 40));
        setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId,
                 juce::Colour(100, 100, 100));
        setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,
                 juce::Colour(200, 200, 100));
        setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,
                 juce::Colour(100, 200, 255).withAlpha(0.6f));
        
        setAvailableRange(36, 96);  // C2 to C7
        setKeyWidth(18.0f);
        setLowestVisibleKey(48);  // C3
    }
};


// ════════════════════════════════════════════════════════════════════════════
// 5. RING MODULATOR MODULE
// ════════════════════════════════════════════════════════════════════════════

#pragma once
#include "Module.h"
#include <cmath>

namespace Modular::Modules
{
    /**
     * Ring Modulator - Amplitude Modulation Effect
     * 
     * Features:
     * - Multiply two signals (input × modulation)
     * - Carrier frequency control
     * - Modulation depth
     * - Multiple waveforms for modulator
     * - Dry/wet mix
     * 
     * Output: Input × (1 + Modulator × Depth)
     */
    
    class RingModulatorModule : public Module
    {
    public:
        enum class ModulatorType
        {
            Sine = 0,
            Triangle = 1,
            Sawtooth = 2,
            Square = 3,
            Noise = 4
        };
        
        RingModulatorModule()
            : carrierPhase_(0.0f),
              carrierFreq_(440.0f),
              modulatorType_(ModulatorType::Sine)
        {
            // Carrier frequency
            ParameterSpec carrierSpec;
            carrierSpec.name = "carrierFreq";
            carrierSpec.displayName = "Carrier Frequency";
            carrierSpec.type = ParameterSpec::Type::Logarithmic;
            carrierSpec.minValue = 10.0f;
            carrierSpec.maxValue = 10000.0f;
            carrierSpec.defaultValue = 440.0f;
            carrierSpec.unit = "Hz";
            registerParameter(carrierSpec);
            
            // Modulator frequency
            ParameterSpec modSpec;
            modSpec.name = "modFreq";
            modSpec.displayName = "Modulator Frequency";
            modSpec.type = ParameterSpec::Type::Logarithmic;
            modSpec.minValue = 0.1f;
            modSpec.maxValue = 5000.0f;
            modSpec.defaultValue = 10.0f;
            modSpec.unit = "Hz";
            registerParameter(modSpec);
            
            // Modulation depth
            ParameterSpec depthSpec;
            depthSpec.name = "depth";
            depthSpec.displayName = "Modulation Depth";
            depthSpec.type = ParameterSpec::Type::Linear;
            depthSpec.minValue = 0.0f;
            depthSpec.maxValue = 1.0f;
            depthSpec.defaultValue = 0.5f;
            registerParameter(depthSpec);
            
            // Modulator type
            ParameterSpec typeSpec;
            typeSpec.name = "modType";
            typeSpec.displayName = "Modulator Type";
            typeSpec.type = ParameterSpec::Type::Choice;
            typeSpec.choices = {"Sine", "Triangle", "Sawtooth", "Square", "Noise"};
            typeSpec.defaultValue = 0.0f;
            registerParameter(typeSpec);
            
            // Dry/wet
            ParameterSpec wetSpec;
            wetSpec.name = "wet";
            wetSpec.displayName = "Wet";
            wetSpec.type = ParameterSpec::Type::Linear;
            wetSpec.minValue = 0.0f;
            wetSpec.maxValue = 1.0f;
            wetSpec.defaultValue = 0.5f;
            registerParameter(wetSpec);
        }
        
        std::string getModuleName() const override { return "RingModulator"; }
        std::string getModuleCategory() const override { return "Effect"; }
        
        void reset() override
        {
            carrierPhase_ = 0.0f;
            modulatorPhase_ = 0.0f;
        }
        
        AudioFrame processFrame(const AudioFrame& input) override
        {
            if (getBypass()) return input;
            
            float carrierFreq = getParameter("carrierFreq");
            float modFreq = getParameter("modFreq");
            float depth = getParameter("depth");
            int typeChoice = (int)getParameter("modType");
            float wet = getParameter("wet");
            
            modulatorType_ = (ModulatorType)typeChoice;
            carrierFreq_ = carrierFreq;
            
            // Generate carrier
            float carrierPhaseInc = carrierFreq / sampleRate_;
            float carrier = std::sin(carrierPhase_ * 6.28318531f);
            carrierPhase_ += carrierPhaseInc;
            if (carrierPhase_ >= 1.0f) carrierPhase_ -= 1.0f;
            
            // Generate modulator
            float modPhaseInc = modFreq / sampleRate_;
            float modulator = generateModulator();
            modulatorPhase_ += modPhaseInc;
            if (modulatorPhase_ >= 1.0f) modulatorPhase_ -= 1.0f;
            
            // Ring modulation: input × (1 + modulator × depth)
            float modulationAmount = 1.0f + modulator * depth;
            
            // Apply both carrier and modulation
            float ringModulated = input.samples[0] * carrier * modulationAmount;
            
            // Mix dry/wet
            float output = input.samples[0] * (1.0f - wet) + ringModulated * wet;
            
            // Soft clip
            if (std::abs(output) > 1.0f)
                output = std::tanh(output);
            
            AudioFrame result = input;
            result.samples[0] = output;
            return result;
        }

    private:
        float carrierPhase_;
        float modulatorPhase_;
        float carrierFreq_;
        ModulatorType modulatorType_;
        
        float generateModulator()
        {
            switch (modulatorType_)
            {
                case ModulatorType::Sine:
                    return std::sin(modulatorPhase_ * 6.28318531f);
                
                case ModulatorType::Triangle:
                {
                    if (modulatorPhase_ < 0.5f)
                        return (modulatorPhase_ * 4.0f) - 1.0f;
                    else
                        return 3.0f - (modulatorPhase_ * 4.0f);
                }
                
                case ModulatorType::Sawtooth:
                    return (modulatorPhase_ * 2.0f) - 1.0f;
                
                case ModulatorType::Square:
                    return (modulatorPhase_ < 0.5f) ? 1.0f : -1.0f;
                
                case ModulatorType::Noise:
                    return ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f;
                
                default:
                    return 0.0f;
            }
        }
    };

} // namespace Modular::Modules


// ════════════════════════════════════════════════════════════════════════════
// 6. COMPLETE UI PANEL (JUCE - All Components Combined)
// ════════════════════════════════════════════════════════════════════════════

class MasterControlPanel : public juce::Component
{
public:
    MasterControlPanel()
        : keyboardState_(),
          keyboard_(keyboardState_, juce::MidiKeyboardComponent::horizontalKeyboard),
          ringModulator_(std::make_unique<RingModulatorModule>())
    {
        // Add all components
        addAndMakeVisible(modWheel_);
        addAndMakeVisible(pitchBender_);
        addAndMakeVisible(oscilloscope_);
        addAndMakeVisible(keyboard_);
        
        // Set up callbacks
        modWheel_.onModWheelChanged = [this](float value) {
            std::cout << "Mod Wheel: " << value << "\n";
        };
        
        pitchBender_.onPitchBendChanged = [this](float value) {
            std::cout << "Pitch Bend: " << value << "\n";
        };
        
        keyboard_.setMidiChannel(1);
        
        setSize(1000, 700);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(40, 40, 50));
    }
    
    void resized() override
    {
        // Layout
        modWheel_.setBounds(10, 10, 80, 250);
        pitchBender_.setBounds(100, 10, 200, 100);
        oscilloscope_.setBounds(310, 10, 500, 250);
        
        keyboard_.setBounds(10, 270, getWidth() - 20, 120);
    }
    
    RingModulatorModule& getRingModulator() { return *ringModulator_; }

private:
    ModWheelComponent modWheel_;
    PitchBenderComponent pitchBender_;
    OscilloscopeComponent oscilloscope_;
    
    juce::MidiKeyboardState keyboardState_;
    KeyboardComponent keyboard_;
    
    std::unique_ptr<RingModulatorModule> ringModulator_;
};
