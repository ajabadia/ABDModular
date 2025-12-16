#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <array>

namespace Modular
{

namespace Core
{

// ════════════════════════════════════════════════════════════════════════════
// AUDIO DATA STRUCTURES
// ════════════════════════════════════════════════════════════════════════════

/**
 * @brief Represents a single frame of audio data (multichannel).
 * Designed for sample-by-sample processing or small block vectorization.
 */
struct AudioFrame
{
    static constexpr int MAX_CHANNELS = 2; // Stereo by default
    std::array<float, MAX_CHANNELS> samples;

    AudioFrame() { clear(); }

    void clear()
    {
        samples.fill(0.0f);
    }

    // Helper: Add another frame to this one
    void add(const AudioFrame& other)
    {
        for (int i = 0; i < MAX_CHANNELS; ++i)
            samples[i] += other.samples[i];
    }

    // Helper: Apply gain
    void multiply(float gain)
    {
        for (int i = 0; i < MAX_CHANNELS; ++i)
            samples[i] *= gain;
    }
};

// ════════════════════════════════════════════════════════════════════════════
// EVENT DATA STRUCTURES
// ════════════════════════════════════════════════════════════════════════════

/**
 * @brief Represents a control event in the modular system.
 * Used for MIDI, automation, and inter-module communication.
 */
struct ControlMessage
{
    enum class Type
    {
        NoteOn,
        NoteOff,
        GateOpen,   // Envelope trigger
        GateClose,  // Envelope release
        ParameterChange,
        Modulation,
        Sysex       // System exclusive data
    };

    Type type;
    int channel = 0;
    int note = 0;           // For NoteOn/Off
    float velocity = 0.0f;  // For NoteOn (0.0 - 1.0)
    
    std::string paramId;    // For ParameterChange
    float value = 0.0f;     // For ParameterChange/Modulation
};

} // namespace Core
} // namespace Modular
