#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <vector>
#include <cmath>

namespace Modular
{
namespace Modules
{

// Helper classes
class DelayLine
{
public:
    void setSize(int size) {
        buffer_.resize(size, 0.0f);
        pos_ = 0;
    }
    float read() const { return buffer_[pos_]; }
    void write(float val) {
        if (buffer_.empty()) return;
        buffer_[pos_] = val;
        pos_++;
        if (pos_ >= (int)buffer_.size()) pos_ = 0;
    }
private:
    std::vector<float> buffer_;
    int pos_ = 0;
};

class Comb
{
public:
    void setDiff(int size, float feedback) {
        delay_.setSize(size);
        feedback_ = feedback;
    }
    float process(float input) {
        float out = delay_.read();
        delay_.write(input + out * feedback_);
        return out;
    }
private:
    DelayLine delay_;
    float feedback_ = 0.0f;
};

class AllPass
{
public:
    void setDiff(int size) {
        delay_.setSize(size);
    }
    float process(float input) {
        float delayed = delay_.read();
        float out = -input + delayed;
        // Allpass feedback coeff usually around 0.5 - 0.7 for reverb
        // y[n] = -x[n] + x[n-D] + g*y[n-D] -- wait standard AP is:
        // y[n] = -g*x[n] + x[n-D] + g*y[n-D]
        // Let's use simpler Schroeder AP:
        // out = delay - gain * input
        // new_delay_in = input + gain * delay
        
        float gain = 0.5f;
        float newDelayIn = input + (delayed * gain);
        delay_.write(newDelayIn);
        
        return delayed - (input * gain); // Verification needed but standard AP structure
    }
private:
    DelayLine delay_;
};

class StandardReverb : public Core::Module
{
public:
    StandardReverb()
    {
        registerParameter("mix", "Mix", 0.0f, 1.0f, 0.3f);
        registerParameter("size", "Room Size", 0.5f, 2.0f, 1.0f);
    }

    std::string getName() const override { return "StandardReverb"; }

    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        Core::Module::prepareToPlay(sampleRate, maxBlockSize);
        
        // Schroeder Reverb tuning (approx ms values converted to samples)
        // 4 Parallel Combs
        // 2 Series AllPass
        
        float scale = sampleRate / 44100.0f;
        
        // Combs: 29.7, 37.1, 41.1, 43.7 ms (Prime numbers approx)
        c1_.setDiff((int)(1310 * scale), 0.773f);
        c2_.setDiff((int)(1636 * scale), 0.802f);
        c3_.setDiff((int)(1811 * scale), 0.753f);
        c4_.setDiff((int)(1927 * scale), 0.733f);
        
        // AllPass: 5.0, 1.7 ms
        ap1_.setDiff((int)(220 * scale));
        ap2_.setDiff((int)(75 * scale));
    }

    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        if (sampleRate_ <= 0.0) return input;
        
        Core::AudioFrame output;
        float mix = getParameter("mix");
        
        // Sum mono input for reverb engine
        float inMono = (input.samples[0] + input.samples[1]) * 0.5f;
        
        // Parallel Combs
        float sum = 0.0f;
        sum += c1_.process(inMono);
        sum += c2_.process(inMono);
        sum += c3_.process(inMono);
        sum += c4_.process(inMono);
        
        // Series AllPass
        float outReverb = ap1_.process(sum);
        outReverb = ap2_.process(outReverb);
        
        // Mix (Wet is outReverb, Dry is input)
        // Simple blend
        output.samples[0] = (input.samples[0] * (1.0f - mix)) + (outReverb * mix * 0.5f);
        output.samples[1] = (input.samples[1] * (1.0f - mix)) + (outReverb * mix * 0.5f);
        
        return output;
    }

private:
    Comb c1_, c2_, c3_, c4_;
    AllPass ap1_, ap2_;
};

} // namespace Modules
} // namespace Modular
