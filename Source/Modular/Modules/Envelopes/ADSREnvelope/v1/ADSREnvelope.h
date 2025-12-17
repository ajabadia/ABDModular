#pragma once

#include "../../../../Core/Foundation/v1/Module.h"

namespace Modular
{
namespace Modules
{

class ADSREnvelope : public Core::Module
{
public:
    enum class State { Idle, Attack, Decay, Sustain, Release };

    ADSREnvelope()
    {
        registerParameter("attack", "Attack", 0.0f, 10.0f, 0.1f);
        registerParameter("decay", "Decay", 0.0f, 10.0f, 0.1f);
        registerParameter("sustain", "Sustain", 0.0f, 1.0f, 0.8f);
        registerParameter("release", "Release", 0.0f, 10.0f, 0.2f);
    }

    std::string getName() const override { return "ADSREnvelope"; }

    void handleMessage(const Core::ControlMessage& message) override
    {
        if (message.type == Core::ControlMessage::Type::GateOpen)
        {
            state_ = State::Attack;
            currentLevel_ = 0.0f; // Re-trigger from zero (or allow legato?)
        }
        else if (message.type == Core::ControlMessage::Type::GateClose)
        {
            state_ = State::Release;
        }
    }

    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        Core::AudioFrame output = input; // Envelopes often process amplitudes of input signal (VCA logic)
                                         // OR valid as a control signal source.
                                         // Let's implement as a VCA: Audio In * Envelope -> Audio Out
        
        float blockRate = 1.0f / static_cast<float>(sampleRate_);
        
        switch (state_)
        {
            case State::Idle:
                currentLevel_ = 0.0f;
                break;

            case State::Attack:
            {
                float attRate = 1.0f / (getParameter("attack") + 0.001f);
                currentLevel_ += attRate * blockRate;
                if (currentLevel_ >= 1.0f)
                {
                    currentLevel_ = 1.0f;
                    state_ = State::Decay;
                }
                break;
            }

            case State::Decay:
            {
                float decRate = 1.0f / (getParameter("decay") + 0.001f);
                float susLevel = getParameter("sustain");
                currentLevel_ -= decRate * blockRate;
                if (currentLevel_ <= susLevel)
                {
                    currentLevel_ = susLevel;
                    state_ = State::Sustain;
                }
                break;
            }

            case State::Sustain:
                currentLevel_ = getParameter("sustain");
                break;

            case State::Release:
            {
                float relRate = 1.0f / (getParameter("release") + 0.001f);
                currentLevel_ -= relRate * blockRate;
                if (currentLevel_ <= 0.0f)
                {
                    currentLevel_ = 0.0f;
                    state_ = State::Idle;
                }
                break;
            }
        }

        output.multiply(currentLevel_);
        return output;
    }

private:
    State state_ = State::Idle;
    float currentLevel_ = 0.0f;
};

} // namespace Modules
} // namespace Modular
