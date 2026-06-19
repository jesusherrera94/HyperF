#pragma once
#include "dspPrimitives.h"
#include <atomic>
#include <memory>

enum class FuzzMode {
    Fuzz_I,     // Mid-forward
    Fuzz_II,    // Scooped
    Gain_Boost  // Clean/Crunch
};

class HyperFuzzDSP {
public:
    HyperFuzzDSP(float sampleRate);

    // Thread-safe parameter setters (called from UI/Hardware loop)
    void setLevel(float level);
    void setGain(float gain) ;
    void setMode(FuzzMode mode);
    void setBass(float bass);      // 0..1, 0.5 = flat
    void setTreble(float treble);  // 0..1, 0.5 = flat

    // Audio Processing block (called from Audio Callback)
    float processSample(float input);

private:
    float sampleRate_;
    dsp::FuzzEngine fuzzEngine_;
    std::unique_ptr<dsp::BiquadFilter> notchFilter_;
    std::unique_ptr<dsp::BiquadFilter> bassShelf_;    // ~110 Hz low shelf
    std::unique_ptr<dsp::BiquadFilter> trebleShelf_;  // ~750 Hz high shelf
    dsp::DCBlocker dcBlocker_;

    // Atomic variables for thread-safe parameter updating
    std::atomic<float> level_{0.5f};
    std::atomic<float> gain_{1.0f};
    std::atomic<float> bass_{0.5f};
    std::atomic<FuzzMode> mode_{FuzzMode::Fuzz_I};
    std::atomic<float> treble_{0.5f};
};