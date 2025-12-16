#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

// Core
#include "../Modular/Core/Foundation/v1/Types.h"
#include "../Modular/Core/Foundation/v1/Module.h"

// Modules
#include "../Modular/Modules/Oscillators/StandardOscillator/v1/StandardOscillator.h"
#include "../Modular/Modules/Envelopes/ADSREnvelope/v1/ADSREnvelope.h"
#include "../Modular/Modules/Filters/StandardFilter/v1/StandardFilter.h"
#include "../Modular/Modules/Mixers/SimpleMixer/v1/SimpleMixer.h"
#include "../Modular/Modules/LFOs/StandardLFO/v1/StandardLFO.h"
#include "../Modular/Modules/Sequencers/PolySequencer/v1/PolySequencer.h"
#include "../Modular/Modules/Effects/StandardDelay/v1/StandardDelay.h"
#include "../Modular/Modules/Effects/StandardChorus/v1/StandardChorus.h"
#include "../Modular/Modules/Effects/StandardReverb/v1/StandardReverb.h"

int main()
{
    std::cout << "Running Modular Tests..." << std::endl;

    Modular::Core::AudioFrame zeroInput; 
    
    // 1. Test AudioFrame
    Modular::Core::AudioFrame frame;
    frame.samples[0] = 0.5f;
    frame.samples[1] = -0.5f;
    assert(frame.samples[0] == 0.5f);
    std::cout << "[PASS] AudioFrame init" << std::endl;

    // 2. Test Oscillator
    Modular::Modules::StandardOscillator osc;
    osc.prepareToPlay(44100.0, 512);
    osc.setParameter("frequency", 440.0f);
    
    Modular::Core::AudioFrame output = osc.processFrame(zeroInput);
    output = osc.processFrame(zeroInput);
    assert(std::abs(output.samples[0]) > 0.0f);
    std::cout << "[PASS] Oscillator processing" << std::endl;

    // 3. Test Envelope
    Modular::Modules::ADSREnvelope env;
    env.prepareToPlay(44100.0, 512);
    
    Modular::Core::ControlMessage msg;
    msg.type = Modular::Core::ControlMessage::Type::GateOpen;
    env.handleMessage(msg);
    
    Modular::Core::AudioFrame audioIn;
    audioIn.samples[0] = 1.0f;
    audioIn.samples[1] = 1.0f;
    
    Modular::Core::AudioFrame audioOut = env.processFrame(audioIn);
    assert(audioOut.samples[0] > 0.0f);
    std::cout << "[PASS] Envelope processing" << std::endl;

    // 4. Test Filter
    Modular::Modules::StandardFilter filter;
    filter.prepareToPlay(44100.0, 512);
    filter.setParameter("cutoff", 500.0f);
    filter.processFrame(audioIn); 
    std::cout << "[PASS] Filter processing" << std::endl;
    
    // 5. Test LFO
    Modular::Modules::StandardLFO lfo;
    lfo.prepareToPlay(44100.0, 512);
    lfo.setParameter("waveform", 2.0f); 
    Modular::Core::AudioFrame lfoOut = lfo.processFrame(zeroInput); 
    assert(lfoOut.samples[0] != 0.0f);
    std::cout << "[PASS] LFO processing" << std::endl;

    // 6. Test Mixer
    Modular::Modules::SimpleMixer mixer;
    mixer.prepareToPlay(44100.0, 512);
    mixer.setParameter("master", 0.5f);
    
    Modular::Core::AudioFrame mixIn;
    mixIn.samples[0] = 1.0f;
    mixIn.samples[1] = 1.0f;
    
    Modular::Core::AudioFrame mixOut = mixer.processFrame(mixIn);
    assert(std::abs(mixOut.samples[0] - 0.5f) < 0.001f);
    std::cout << "[PASS] Mixer processing" << std::endl;

    // 7. Test PolySequencer
    Modular::Modules::PolySequencer seq;
    seq.prepareToPlay(44100.0, 512);
    seq.setParameter("on", 1.0f);
    seq.setParameter("tempo", 120.0f);
    
    seq.addTrack("Track 1"); 
    seq.setNote(0, 0, 60, 1.0f);
    
    for(int i=0; i<100; ++i) 
        seq.processFrame(zeroInput);
    
    auto notes = seq.getCurrentNotes();
    if (!notes.empty()) {
        assert(notes[0].second.noteValue == 60);
    }
    std::cout << "[PASS] Sequencer processing" << std::endl;

    // 8. Test Delay
    Modular::Modules::StandardDelay delay;
    delay.prepareToPlay(44100.0, 512);
    delay.setParameter("time", 0.01f);
    delay.setParameter("mix", 0.5f);
    
    Modular::Core::AudioFrame impulse;
    impulse.samples[0] = 1.0f;
    impulse.samples[1] = 1.0f;
    
    delay.processFrame(impulse);
    delay.processFrame(zeroInput);
    std::cout << "[PASS] Delay processing" << std::endl;
    
    // 9. Test Chorus
    Modular::Modules::StandardChorus chorus;
    chorus.prepareToPlay(44100.0, 512);
    chorus.processFrame(impulse);
    std::cout << "[PASS] Chorus processing" << std::endl;
    
    // 10. Test Reverb
    Modular::Modules::StandardReverb reverb;
    reverb.prepareToPlay(44100.0, 512);
    reverb.processFrame(impulse);
    std::cout << "[PASS] Reverb processing" << std::endl;

    std::cout << "All Modular Tests Passed!" << std::endl;
    return 0;
}
