#pragma once

#include "../../../../Core/Foundation/v1/Module.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

namespace Modular
{
namespace Modules
{

class PolySequencer : public Core::Module
{
public:
    static constexpr int MAX_STEPS = 64;
    static constexpr int MAX_TRACKS = 16;

    struct Note
    {
        int noteValue = 60;
        float velocity = 0.8f;
        float duration = 0.5f;
        bool active = false;
    };

    struct Track
    {
        std::string name;
        bool muted = false;
        bool solo = false;
        int currentStep = 0;
        int stepCount = 16;
        std::vector<Note> steps;

        Track(const std::string& n) : name(n) {
            steps.resize(MAX_STEPS);
        }
    };

    enum class PlayMode { Forward, Backward, Random };

    PolySequencer()
    {
        // Parameters
        registerParameter("tempo", "Tempo", 40.0f, 300.0f, 120.0f);
        registerParameter("on", "Run", 0.0f, 1.0f, 0.0f);
        registerParameter("mode", "Mode", 0.0f, 2.0f, 0.0f);
        
        // Default track
        addTrack("Track 1");
    }

    std::string getName() const override { return "PolySequencer"; }

    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        Core::Module::prepareToPlay(sampleRate, maxBlockSize);
        // Reset sub-sample counters if needed
    }

    // Sequencers don't process audio input usually, but they must implement processFrame
    // They emit events (ControlMessage) or write to control outputs/buffers.
    // For this architecture, we will return the input passed through, 
    // BUT we need a way to emit MIDI/Notes.
    // Since `processFrame` returns `AudioFrame`, this sequencer is currently just internal logic 
    // unless we bind it to a synthesizer.
    // In a real usage, we would use `getCurrentNotes()` or have it dispatch `ControlMessage` events queue.
    // REQUIRED: We need to implement `processFrame` to advance the clock.

    Core::AudioFrame processFrame(const Core::AudioFrame& input) override
    {
        if (getParameter("on") < 0.5f) return input;
        
        float tempo = getParameter("tempo");
        // Calculate samples per tick/step (assuming 16th notes: 4 steps per beat)
        // Beats per second = BPM / 60
        // Seconds per beat = 60 / BPM
        // Seconds per 16th = (60/BPM) / 4
        
        if (tempo < 1.0f) tempo = 120.0f;
        
        double samplesPerStep = (sampleRate_ * 60.0 / tempo) / 4.0;
        
        clockAccumulator_ += 1.0;
        
        if (clockAccumulator_ >= samplesPerStep)
        {
            clockAccumulator_ -= samplesPerStep;
            advanceSteps();
        }
        
        return input;
    }
    
    // API for programming (to be used by Editor/UI)
    int addTrack(const std::string& name) {
        if (tracks_.size() >= MAX_TRACKS) return -1;
        tracks_.emplace_back(name);
        return (int)tracks_.size() - 1;
    }
    
    void setNote(int trackIdx, int step, int note, float vel) {
        if (trackIdx < 0 || trackIdx >= tracks_.size()) return;
        if (step < 0 || step >= MAX_STEPS) return;
        
        auto& n = tracks_[trackIdx].steps[step];
        n.noteValue = note;
        n.velocity = vel;
        n.active = true;
    }
    
    // For retrieval by Orchestrator
    std::vector<std::pair<int, Note>> getCurrentNotes() const {
        std::vector<std::pair<int, Note>> activeNotes;
        if (getParameter("on") < 0.5f) return activeNotes;

        // Logic to return flags? 
        // In a pull-based system, the synth asks "what's playing?".
        // Only return if we JUST entered this step? 
        // For now, let's just return the state of the current step for each track.
        
        for (int i=0; i<tracks_.size(); ++i) {
            const auto& t = tracks_[i];
            if (t.muted) continue;
            
            // Check if step valid
             if (t.currentStep < t.stepCount) {
                 const auto& n = t.steps[t.currentStep];
                 if (n.active) {
                     activeNotes.push_back({i, n});
                 }
             }
        }
        return activeNotes;
    }
    
    // Public for test inspection
    int getCurrentStep(int trackIdx) const {
        if (trackIdx >= 0 && trackIdx < tracks_.size())
            return tracks_[trackIdx].currentStep;
        return -1;
    }

private:
    std::vector<Track> tracks_;
    double clockAccumulator_ = 0.0;
    
    void advanceSteps() {
        int mode = (int)getParameter("mode");
        
        for (auto& track : tracks_) {
            if (mode == 1) { // Backward
                track.currentStep--;
                if (track.currentStep < 0) track.currentStep = track.stepCount - 1;
            } else if (mode == 2) { // Random
                track.currentStep = rand() % track.stepCount;
            } else { // Forward
                track.currentStep++;
                if (track.currentStep >= track.stepCount) track.currentStep = 0;
            }
        }
    }
};

} // namespace Modules
} // namespace Modular
