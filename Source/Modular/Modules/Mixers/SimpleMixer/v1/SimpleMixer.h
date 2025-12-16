#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <vector>

namespace Modular
{
namespace Modules
{

class SimpleMixer : public Core::Module
{
public:
    // Supports 2 stereo inputs mix for now (A/B style) plus master
    
    SimpleMixer()
    {
        parameters_["gainA"] = 1.0f;
        parameters_["gainB"] = 1.0f;
        parameters_["master"] = 1.0f;
    }

    // Mixer needs explicit input management. 
    // In our single-chain `processFrame(input)` architecture, a module usually modifies 'input'.
    // A mixer implies SUMMING multiple sources. 
    // For this basic brick, let's assume valid usage is:
    // 1. We might have "Aux" inputs stored or injected.
    // However, adhering to the interface processFrame(input), this mixer acts more like a 
    // "Gain/Balance" stage for the passed signal if typical chain, OR
    // we need a way to sum.
    
    // For V1 modular chain: The "Input" to processFrame represents the MAIN bus up to this point.
    // If we want to mix another signal, we need a method `processMix(inputA, inputB)`.
    // But since we must adhere to `processFrame(oneInput)`, the standard usage in a linear chain
    // is just Volume Control.
    
    // To support real mixing in this architecture, the Orchestrator holds the buffers of modules 
    // and sums them manually. 
    // A "MixerModule" in this context is just a VCA (Voltage Controlled Amplifier) bank.
    
    // Let's implement it as a 2-channel VCA that applies gain to the incoming frame.
    // Useful for sub-mixing voices.
    
    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        Core::AudioFrame output = input;
        
        float gain = getParameter("master");
        // We will treat "gainA" as L channel and "gainB" as R channel balance if we want,
        // or just apply global master gain.
        // Let's stick to simple Gain for now.
        
        output.samples[0] *= gain;
        output.samples[1] *= gain;
        
        return output;
    }
};

} // namespace Modules
} // namespace Modular
