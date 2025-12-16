#pragma once

#include "Types.h"
#include <unordered_map>
#include <string>
#include <iostream>

namespace Modular
{
namespace Core
{

/**
 * @brief Base class for all audio modules in the system.
 * 
 * Provides a unified interface for:
 * - Audio processing (processFrame)
 * - Event handling (handleMessage)
 * - Parameter management
 * 
 * DESIGN PRINCIPLE:
 * Modules are isolated signal processors. They don't know about the
 * global orchestrator or other modules unless explicitly connected.
 */
class Module
{
public:
    virtual ~Module() = default;

    /**
     * @brief Prepares the module for playback.
     * @param sampleRate The audio sample rate (e.g., 44100.0 or 48000.0)
     * @param maxBlockSize The maximum number of samples per block (hint)
     */
    virtual void prepareToPlay(double sampleRate, int maxBlockSize)
    {
        sampleRate_ = sampleRate;
        maxBlockSize_ = maxBlockSize;
        // Subclasses can override to initialize buffers, etc.
    }

    /**
     * @brief Process a single frame of audio.
     * This is the core signal processing method.
     * 
     * @param input The input signal from the previous module.
     * @return The processed output signal.
     */
    virtual AudioFrame processFrame(const AudioFrame& input) = 0;

    /**
     * @brief Handle a control message (MIDI, automation, etc.).
     * @param message The message to process.
     */
    virtual void handleMessage(const ControlMessage& message)
    {
        // Base implementation handles common tasks if any.
        // Subclasses should override to handle specific events.
    }

    /**
     * @brief Sets a generic floating-point parameter.
     */
    virtual void setParameter(const std::string& paramId, float value)
    {
        if (parameters_.find(paramId) != parameters_.end())
        {
            parameters_[paramId] = value;
        }
        else
        {
            // Auto-create parameter if it doesn't exist (flexible)
            // Or log warning in strict mode.
            parameters_[paramId] = value;
        }
    }

    /**
     * @brief Gets a parameter value. Returns 0.0f if not found.
     */
    virtual float getParameter(const std::string& paramId) const
    {
        auto it = parameters_.find(paramId);
        if (it != parameters_.end())
            return it->second;
        return 0.0f;
    }

    /**
     * @brief Resets the internal state (e.g., clear buffers).
     */
    virtual void reset() {}

protected:
    double sampleRate_ = 0.0;
    int maxBlockSize_ = 0;
    
    // Generic local parameter storage.
    // In a real implementation, we might want atomic float pointers 
    // for thread safety, but for this modular prototype, a map is sufficient
    // provided we update it carefully or use lock-free concepts later.
    std::unordered_map<std::string, float> parameters_;
};

} // namespace Core
} // namespace Modular
