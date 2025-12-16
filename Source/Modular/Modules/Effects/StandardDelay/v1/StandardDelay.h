#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <vector>
#include <algorithm>

namespace Modular
{
namespace Modules
{

class StandardDelay : public Core::Module
{
public:
    StandardDelay()
    {
        parameters_["time"] = 0.5f;     // Seconds (500ms)
        parameters_["feedback"] = 0.3f; // 0..1
        parameters_["mix"] = 0.5f;      // Dry/Wet
    }

    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        Core::Module::prepareToPlay(sampleRate, maxBlockSize);
        // Resize buffer to 2 seconds max
        int size = (int)(sampleRate * 2.0) + maxBlockSize;
        if (size < 1) size = 44100;
        
        bufferL_.resize(size, 0.0f);
        bufferR_.resize(size, 0.0f);
        writePos_ = 0;
        
        // Clear buffers
        std::fill(bufferL_.begin(), bufferL_.end(), 0.0f);
        std::fill(bufferR_.begin(), bufferR_.end(), 0.0f);
    }

    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        if (sampleRate_ <= 0.0 || bufferL_.empty()) return input;

        Core::AudioFrame output;
        
        float timeParam = getParameter("time");
        float feedbackCallback = getParameter("feedback");
        float mix = getParameter("mix");

        // Calculate delay in samples
        float delaySamples = timeParam * (float)sampleRate_;
        if (delaySamples < 1.0f) delaySamples = 1.0f;
        if (delaySamples > bufferL_.size() - 1) delaySamples = (float)bufferL_.size() - 1;

        // Read pointer
        // Simple linear interpolation could go here, for now nearest integer
        float readIndex = (float)writePos_ - delaySamples;
        while (readIndex < 0.0f) readIndex += (float)bufferL_.size();
        
        int rPos = (int)readIndex;
        
        // Read delayed signal
        float delayedL = bufferL_[rPos];
        float delayedR = bufferR_[rPos];
        
        // Write to buffer (Input + Feedback)
        float nextL = input.samples[0] + (delayedL * feedbackCallback);
        float nextR = input.samples[1] + (delayedR * feedbackCallback);
        
        // Soft clip to prevent explosion
        if (nextL > 2.0f) nextL = 2.0f; if (nextL < -2.0f) nextL = -2.0f;
        if (nextR > 2.0f) nextR = 2.0f; if (nextR < -2.0f) nextR = -2.0f;

        bufferL_[writePos_] = nextL;
        bufferR_[writePos_] = nextR;

        // Output mix
        output.samples[0] = (input.samples[0] * (1.0f - mix)) + (delayedL * mix);
        output.samples[1] = (input.samples[1] * (1.0f - mix)) + (delayedR * mix);

        // Advance write pointer
        writePos_++;
        if (writePos_ >= (int)bufferL_.size()) writePos_ = 0;

        return output;
    }

private:
    std::vector<float> bufferL_;
    std::vector<float> bufferR_;
    int writePos_ = 0;
};

} // namespace Modules
} // namespace Modular
