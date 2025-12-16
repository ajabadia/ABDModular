#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <vector>
#include <cmath>
#include <algorithm>

namespace Modular
{
namespace Modules
{

class StandardChorus : public Core::Module
{
public:
    StandardChorus()
    {
        parameters_["rate"] = 0.5f;     // Hz
        parameters_["depth"] = 0.005f;  // Seconds (5ms modulation)
        parameters_["delay"] = 0.020f;  // Base delay 20ms
        parameters_["mix"] = 0.5f;
        parameters_["feedback"] = 0.2f;
    }

    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        Core::Module::prepareToPlay(sampleRate, maxBlockSize);
        
        // 50ms buffer is enough for chorus
        int size = (int)(sampleRate * 0.1) + maxBlockSize; 
        bufferL_.resize(size, 0.0f);
        bufferR_.resize(size, 0.0f);
        writePos_ = 0;
        lfoPhase_ = 0.0f;
    }

    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        if (sampleRate_ <= 0.0 || bufferL_.empty()) return input;
        
        Core::AudioFrame output;
        
        float rate = getParameter("rate");
        float depthTime = getParameter("depth");
        float baseDelayTime = getParameter("delay");
        float mix = getParameter("mix");
        float fb = getParameter("feedback");
        
        // Update LFO
        float phaseInc = (rate * 6.2831853f) / (float)sampleRate_;
        lfoPhase_ += phaseInc;
        if (lfoPhase_ > 6.2831853f) lfoPhase_ -= 6.2831853f;
        
        float lfoVal = std::sin(lfoPhase_);
        
        // Calculate read positions
        // Modulate delay time: Base + (Depth * LFO)
        // Stereo spread: Invert LFO for Right channel (simple 180 degrees)
        
        float modL = lfoVal * depthTime;
        float modR = -lfoVal * depthTime; // Wide stereo
        
        float delaySamplesL = (baseDelayTime + modL) * (float)sampleRate_;
        float delaySamplesR = (baseDelayTime + modR) * (float)sampleRate_;
        
        // Read L
        float readPosL = (float)writePos_ - delaySamplesL;
        while (readPosL < 0.0f) readPosL += (float)bufferL_.size();
        float valL = interpolate(bufferL_, readPosL);
        
        // Read R
        float readPosR = (float)writePos_ - delaySamplesR;
        while (readPosR < 0.0f) readPosR += (float)bufferR_.size();
        float valR = interpolate(bufferR_, readPosR);
        
        // Write (Input + Feedback)
        float nextL = input.samples[0] + (valL * fb);
        float nextR = input.samples[1] + (valR * fb);
        
        bufferL_[writePos_] = nextL;
        bufferR_[writePos_] = nextR;
        
        // Mix
        output.samples[0] = (input.samples[0] * (1.0f - mix)) + (valL * mix);
        output.samples[1] = (input.samples[1] * (1.0f - mix)) + (valR * mix);
        
        writePos_++;
        if (writePos_ >= (int)bufferL_.size()) writePos_ = 0;
        
        return output;
    }

private:
    std::vector<float> bufferL_;
    std::vector<float> bufferR_;
    int writePos_ = 0;
    float lfoPhase_ = 0.0f;
    
    // Linear interpolation
    float interpolate(const std::vector<float>& buffer, float pos) {
        int idx = (int)pos;
        float frac = pos - (float)idx;
        int next = idx + 1;
        if (next >= (int)buffer.size()) next = 0;
        return buffer[idx] * (1.0f - frac) + buffer[next] * frac;
    }
};

} // namespace Modules
} // namespace Modular
