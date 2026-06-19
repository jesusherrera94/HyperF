#pragma once
#include "hothouse.h"
#include "FuzzProcessor.h"
// Adapter: the ONLY place that knows about Hothouse/DaisySeed.
// It translates hardware I/O to/from the platform-agnostic HyperFuzzDSP,
// keeping the DSP engine reusable (e.g. in JUCE).
class HothouseAdapter {
public:
    HothouseAdapter(clevelandmusicco::Hothouse& hw, HyperFuzzDSP& dsp);
    // Reads knobs/switches and pushes them into the DSP. Call from control loop.
    void updateControls();
    // Registered with Hothouse::StartAudio. Must be a static/free function
    // because Daisy expects a C-style function pointer.
    static void AudioCallback(daisy::AudioHandle::InputBuffer in,
                              daisy::AudioHandle::OutputBuffer out,
                              size_t size);
    // True bypass state for external use (e.g. LED). Optional getter.
    bool isBypassed() const { return bypassed_.load(std::memory_order_relaxed); }
private:
    clevelandmusicco::Hothouse& hw_;
    HyperFuzzDSP& dsp_;
    // File-scope processor used by the static AudioCallback.
    static HyperFuzzDSP* audioDsp_;
    static std::atomic<bool> bypassed_;  // Shared between the control loop (writer) and audio callback (reader).
    daisy::Led led_bypass_;
};
