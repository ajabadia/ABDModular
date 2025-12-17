#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <cmath>

namespace Modular
{
namespace Modules
{

class StandardOscillator : public Core::Module
{
public:
    enum class Waveform { Sine, Triangle, Saw, Square, Noise };

    StandardOscillator()
    {
        // Initialize default parameters with ranges
        registerParameter("frequency", "Frequency", 20.0f, 20000.0f, 440.0f);
        registerParameter("detune", "Detune", -12.0f, 12.0f, 0.0f);
        registerParameter("waveform", "Waveform", 0.0f, 4.0f, 0.0f);
        registerParameter("level", "Level", 0.0f, 1.0f, 1.0f);
    }

    std::string getName() const override { return "StandardOscillator"; }

    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        Module::prepareToPlay(sampleRate, maxBlockSize);
        updatePhaseIncrement();
    }

    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        Core::AudioFrame output;
        
        // Simple oscillator logic
        float currentSample = 0.0f;

        // Retrieve waveform type
        int waveType = static_cast<int>(getParameter("waveform"));
        
        // 1. Calculate phase
        // (In real impl, use wavetables or polyblep for anti-aliasing)
        
        switch (static_cast<Waveform>(waveType))
        {
            case Waveform::Sine:
                currentSample = std::sin(currentPhase_);
                break;
            case Waveform::Saw:
                currentSample = 1.0f - (2.0f * (currentPhase_ / 6.283185307f));
                break;
            case Waveform::Square:
                currentSample = (currentPhase_ < 3.14159265f) ? 1.0f : -1.0f;
                break;
            case Waveform::Triangle:
            {
               float t = currentPhase_ / 6.283185307f;
               currentSample = 2.0f * std::abs(2.0f * (t - std::floor(t + 0.5f))) - 1.0f;
               break;
            }
            default: // Sine fallback
                currentSample = std::sin(currentPhase_); 
                break;
        }

        // Increment phase
        currentPhase_ += phaseIncrement_;
        if (currentPhase_ >= 6.283185307f)
            currentPhase_ -= 6.283185307f;

        // Apply input modulation (Frequency Modulation could happen here)
        // For now, simple mixing if needed, or ignoring input if we are a source
        // Let's assume input might be FM or Sync? 
        // For basic osc, we typically ignore input (or treat as FM).
        
        float level = getParameter("level");
        
        output.samples[0] = currentSample * level;
        output.samples[1] = currentSample * level; // Mono to stereo

        return output;
    }

    void setParameter(const std::string& paramId, float value) override
    {
        Module::setParameter(paramId, value);
        if (paramId == "frequency")
            updatePhaseIncrement();
    }

private:
    float currentPhase_ = 0.0f;
    float phaseIncrement_ = 0.0f;

    void updatePhaseIncrement()
    {
        float freq = getParameter("frequency");
        if (sampleRate_ > 0.0)
            phaseIncrement_ = (freq * 6.283185307f) / static_cast<float>(sampleRate_);
    }
};

} // namespace Modules
} // namespace Modular
