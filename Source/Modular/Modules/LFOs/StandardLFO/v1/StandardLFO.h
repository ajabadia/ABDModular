#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <cmath>

namespace Modular
{
namespace Modules
{

class StandardLFO : public Core::Module
{
public:
    enum class Waveform { Sine, Triangle, Saw, Square, S_H };

    StandardLFO()
    {
        registerParameter("frequency", "Frequency", 0.1f, 20.0f, 1.0f);
        registerParameter("depth", "Depth", 0.0f, 1.0f, 1.0f);
        registerParameter("waveform", "Waveform", 0.0f, 4.0f, 0.0f);
    }

    std::string getName() const override { return "StandardLFO"; }

    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        Core::Module::prepareToPlay(sampleRate, maxBlockSize);
        updatePhaseIncrement();
    }

    void setParameter(const std::string& paramId, float value) override
    {
        Core::Module::setParameter(paramId, value);
        if (paramId == "frequency") updatePhaseIncrement();
    }

    // LFO usually ignores audio input and generates control signal
    // It outputs the LFO value into the audio frame (as DC offset signal)
    // so it can be routed to modulate other params.
    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        Core::AudioFrame output; // output replaces input (LFO is a source)
        
        float currentSample = 0.0f;
        int waveType = static_cast<int>(getParameter("waveform"));

        switch (static_cast<Waveform>(waveType))
        {
            case Waveform::Sine:
                currentSample = std::sin(currentPhase_);
                break;
            case Waveform::Saw: // Rising
                currentSample = -1.0f + (2.0f * (currentPhase_ / 6.283185307f));
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
            default:
                currentSample = std::sin(currentPhase_);
                break;
        }
        
        // Phase increment
        currentPhase_ += phaseIncrement_;
        if (currentPhase_ >= 6.283185307f) currentPhase_ -= 6.283185307f;

        // Apply depth
        float val = currentSample * getParameter("depth");

        // Output to both channels
        output.samples[0] = val;
        output.samples[1] = val;

        return output;
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
