#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <cmath>
#include <numbers>

namespace Modular
{
namespace Modules
{

class StandardFilter : public Core::Module
{
public:
    enum class FilterMode { LowPass, HighPass, BandPass };

    StandardFilter()
    {
        registerParameter("cutoff", "Cutoff", 20.0f, 20000.0f, 20000.0f);
        registerParameter("resonance", "Resonance", 0.0f, 0.99f, 0.0f);
        registerParameter("mode", "Mode", 0.0f, 2.0f, 0.0f); // 0=LP, 1=HP, 2=BP
    }
    
    std::string getName() const override { return "StandardFilter"; }

    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        Core::Module::prepareToPlay(sampleRate, maxBlockSize);
        reset();
        calculateCoefficients();
    }

    void reset() override
    {
        // Clear history buffers for 2 channels
        for (int i = 0; i < 2; ++i) {
            z1_[i] = 0.0f;
            z2_[i] = 0.0f;
        }
    }

    void setParameter(const std::string& paramId, float value) override
    {
        Core::Module::setParameter(paramId, value);
        if (paramId == "cutoff" || paramId == "resonance" || paramId == "mode")
        {
            calculateCoefficients();
        }
    }

    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        Core::AudioFrame output;
        
        // Ensure coefficients are valid
        if (sampleRate_ <= 0.0) return input; // Bypass if not initialized
        
        // Process stereo channels
        for (int ch = 0; ch < 2; ++ch)
        {
            float in = input.samples[ch];
            
            // Biquad difference equation:
            // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
            // Standard naming: input history is implicitly handled effectively by Direct Form II or Transposed
            // But here let's use Direct Form I for clarity:
            
            // Using Direct Form I or II requires separate history for Inputs (x) and Outputs (y).
            // Let's use a simpler State Variable Filter (SVF) or simple Biquad Implementation.
            // Using Biquad Direct Form I:
            // Need x1, x2, y1, y2 history per channel.
             
            // Refactored for simplicity: using buffers z1, z2 for Direct Form II used often in JUCE IIR
            // out = b0*in + z1; 
            // z1 = b1*in - a1*out + z2; 
            // z2 = b2*in - a2*out;
            
            float out = b0_ * in + z1_[ch];
            z1_[ch] = b1_ * in - a1_ * out + z2_[ch];
            z2_[ch] = b2_ * in - a2_ * out;
            
            output.samples[ch] = out;
        }
        
        return output;
    }

private:
    // Biquad coefficients
    float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f;
    float a1_ = 0.0f, a2_ = 0.0f;

    // State (Direct Form II transposedish)
    float z1_[2] = {0.0f, 0.0f};
    float z2_[2] = {0.0f, 0.0f};

    void calculateCoefficients()
    {
        if (sampleRate_ <= 0.0) return;

        float cutoff = getParameter("cutoff");
        float res = getParameter("resonance");
        int mode = static_cast<int>(getParameter("mode"));
        
        // Safety clamps
        if (cutoff < 20.0f) cutoff = 20.0f;
        if (cutoff > sampleRate_ * 0.49f) cutoff = (float)sampleRate_ * 0.49f;
        
        // Q factor derived from resonance (0..1 -> 0.707 .. 10.0)
        float q = 0.707f + (res * 9.0f);

        float w0 = (2.0f * 3.14159265f * cutoff) / (float)sampleRate_;
        float alpha = std::sin(w0) / (2.0f * q);
        float cosW0 = std::cos(w0);

        float a0_inv = 1.0f / (1.0f + alpha);

        if (mode == 0) // LowPass
        {
            b0_ = ((1.0f - cosW0) * 0.5f) * a0_inv;
            b1_ = (1.0f - cosW0) * a0_inv;
            b2_ = ((1.0f - cosW0) * 0.5f) * a0_inv;
            a1_ = (-2.0f * cosW0) * a0_inv;
            a2_ = (1.0f - alpha) * a0_inv;
        }
        else if (mode == 1) // HighPass
        {
            b0_ = ((1.0f + cosW0) * 0.5f) * a0_inv;
            b1_ = -(1.0f + cosW0) * a0_inv;
            b2_ = ((1.0f + cosW0) * 0.5f) * a0_inv;
            a1_ = (-2.0f * cosW0) * a0_inv;
            a2_ = (1.0f - alpha) * a0_inv;
        }
        else // BandPass
        {
            b0_ = alpha * a0_inv;
            b1_ = 0.0f;
            b2_ = -alpha * a0_inv;
            a1_ = (-2.0f * cosW0) * a0_inv;
            a2_ = (1.0f - alpha) * a0_inv;
        }
    }
};

} // namespace Modules
} // namespace Modular
