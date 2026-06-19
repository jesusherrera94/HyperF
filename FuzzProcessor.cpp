#include "dspPrimitives.h"
#include "FuzzProcessor.h"
#include <cmath>
#include <memory>

HyperFuzzDSP::HyperFuzzDSP(float sampleRate) : sampleRate_(sampleRate) {
    notchFilter_ = std::make_unique<dsp::BiquadFilter>();
    bassShelf_   = std::make_unique<dsp::BiquadFilter>();
    trebleShelf_ = std::make_unique<dsp::BiquadFilter>();
    // Initialize Notch for Fuzz II at 1.2kHz
    notchFilter_->calculateNotch(sampleRate_, 1200.0f, 1.5f); 
    bassShelf_ ->calculateLowShelf(sampleRate_, 110.0f, 0.0f);   // flat at startup
    trebleShelf_->calculateHighShelf(sampleRate_, 750.0f, 0.0f);
    dcBlocker_.prepare(sampleRate_, 20.0f);
}

void HyperFuzzDSP::setLevel(float level) {
    level_.store(level, std::memory_order_relaxed);
}

void HyperFuzzDSP::setGain(float gain) {
    gain_.store(gain, std::memory_order_relaxed);
}

void HyperFuzzDSP::setMode(FuzzMode mode) {
    mode_.store(mode, std::memory_order_relaxed);
}

void HyperFuzzDSP::setBass(float bass) {
    bass_.store(bass, std::memory_order_relaxed);
    // Recompute coefficients at control rate (not in the audio thread)
    bassShelf_->calculateLowShelf(sampleRate_, 110.0f, (bass - 0.5f) * 24.0f);
}
void HyperFuzzDSP::setTreble(float treble) {
    treble_.store(treble, std::memory_order_relaxed);
    trebleShelf_->calculateHighShelf(sampleRate_, 750.0f, (treble - 0.5f) * 24.0f);
}


float HyperFuzzDSP::processSample(float input) {
    // Load atomic variables with relaxed memory order for fast thread-safe reading
        float currentGain = gain_.load(std::memory_order_relaxed);
        float currentLevel = level_.load(std::memory_order_relaxed);
        FuzzMode currentMode = mode_.load(std::memory_order_relaxed);

        float output = input;

        switch (currentMode) {
            case FuzzMode::Fuzz_I:
                output = fuzzEngine_.process(input, currentGain);
                // Apply mild shaping filter here
                break;

            case FuzzMode::Fuzz_II:
                output = fuzzEngine_.process(input, currentGain);
                // Apply massive 16dB cut at 1.2kHz
                output = notchFilter_->process(output);
                break;

            case FuzzMode::Gain_Boost:
                // Bypass fuzz, act as a dirty preamp
                output = std::tanh(input * (currentGain * 2.0f)); 
                break;
        }

        // Remove rectifier DC offset before tone shaping.
        output = dcBlocker_.process(output);

        // Active EQ (Bass/Treble)
        output = bassShelf_->process(output);
        output = trebleShelf_->process(output);

        return output * currentLevel;
}