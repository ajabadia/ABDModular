#pragma once

#include <vector>
#include <deque>
#include <algorithm>

namespace Modular
{
namespace Core
{

/**
 * @brief Manages the logic of polyphonic voice allocation.
 * 
 * It tracks which voices are active, free, or stealing candidates.
 * It does NOT contain the actual voice objects (Modules), only their indices.
 * This allows reusing the logic for any type of synthesizer.
 */
class VoiceManager
{
public:
    struct VoiceInfo
    {
        int voiceIndex;
        int midiNote;
        float velocity;
        bool isActive;
        long long lastTriggerTimestamp; // For stealing oldest
    };

    explicit VoiceManager(int numVoices)
    {
        resize(numVoices);
    }

    void resize(int numVoices)
    {
        voices_.resize(numVoices);
        for (int i = 0; i < numVoices; ++i)
        {
            voices_[i] = { i, -1, 0.0f, false, 0 };
        }
    }

    /**
     * @brief Finds a free voice for a new note. 
     * If no voice is free, it steals the oldest one (round-robin / LRU).
     * @return The index of the voice to use.
     */
    int findVoiceIndex(int midiNote, float velocity)
    {
        // 1. Check if note is already playing (retrigger same voice)
        for (auto& v : voices_)
        {
            if (v.isActive && v.midiNote == midiNote)
            {
                updateVoice(v.voiceIndex, midiNote, velocity);
                return v.voiceIndex; // Retrigger
            }
        }

        // 2. Find free voice
        for (auto& v : voices_)
        {
            if (!v.isActive)
            {
                updateVoice(v.voiceIndex, midiNote, velocity);
                return v.voiceIndex;
            }
        }

        // 3. Steal oldest voice
        int lruIndex = 0;
        long long minTime = -1;

        for (int i = 0; i < (int)voices_.size(); ++i)
        {
            // Initializing minTime with first active voice found
            if (minTime == -1 || voices_[i].lastTriggerTimestamp < minTime)
            {
                minTime = voices_[i].lastTriggerTimestamp;
                lruIndex = i;
            }
        }

        updateVoice(lruIndex, midiNote, velocity);
        return lruIndex;
    }

    /**
     * @brief Releases a voice associated with a note.
     * @return The index of the released voice, or -1 if not found.
     */
    int noteOff(int midiNote)
    {
        for (auto& v : voices_)
        {
            if (v.isActive && v.midiNote == midiNote)
            {
                // Note: We don't mark it inactive immediately usually, 
                // because the envelope needs to release. 
                // But for the logic of the manager, we consider the KEY released.
                // The actual "voice free" state should be triggered by the envelope finishing.
                // For this simple manager, we'll keep tracking it as "key down".
                // TODO: Differentiate KeyDown vs VoiceActive.
                // For now, we will perform a simple lookup.
                return v.voiceIndex; 
            }
        }
        return -1;
    }

    void setVoiceFree(int voiceIndex)
    {
        if (voiceIndex >= 0 && voiceIndex < (int)voices_.size())
        {
            voices_[voiceIndex].isActive = false;
            voices_[voiceIndex].midiNote = -1;
        }
    }
    
    const VoiceInfo& getVoiceInfo(int index) const
    {
        return voices_[index];
    }
    
    int getNumVoices() const { return (int)voices_.size(); }

private:
    std::vector<VoiceInfo> voices_;
    long long currentTimestamp_ = 0;

    void updateVoice(int index, int note, float vel)
    {
        voices_[index].isActive = true;
        voices_[index].midiNote = note;
        voices_[index].velocity = vel;
        voices_[index].lastTriggerTimestamp = ++currentTimestamp_;
    }
};

} // namespace Core
} // namespace Modular
