#include "HotHouseAdapter.h"

using clevelandmusicco::Hothouse;
using daisy::AudioHandle;
HyperFuzzDSP* HothouseAdapter::audioDsp_ = nullptr;

std::atomic<bool> HothouseAdapter::bypassed_{true};

HothouseAdapter::HothouseAdapter(Hothouse& hw, HyperFuzzDSP& dsp)
    : hw_(hw), dsp_(dsp) {
    // Bind the instance for the static audio callback.
    audioDsp_ = &dsp;
    led_bypass_.Init(hw.seed.GetPin(Hothouse::LED_2), false);
}
void HothouseAdapter::updateControls() {
    // Refresh ADC + switch readings before mapping.
    hw_.ProcessAllControls();

    // Toggle bypass when FOOTSWITCH_2 is pressed.
    if (hw_.switches[Hothouse::FOOTSWITCH_2].RisingEdge()) {
        bypassed_.store(!bypassed_.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);
    }

    // Hothouse mapping (per spec):
    dsp_.setLevel (hw_.GetKnobValue(Hothouse::KNOB_1));            // Level   0..1
    dsp_.setBass  (hw_.GetKnobValue(Hothouse::KNOB_2));            // Bass    0..1 (0.5 flat)
    dsp_.setTreble(hw_.GetKnobValue(Hothouse::KNOB_3));            // Treble  0..1 (0.5 flat)
    // Gain knob scaled 1.0..50.0 for the heavy FZ-2 compression range.
    dsp_.setGain  (1.0f + hw_.GetKnobValue(Hothouse::KNOB_4) * 49.0f);
    // 3-way rotary -> three voices.
    switch (hw_.GetToggleswitchPosition(Hothouse::TOGGLESWITCH_1)) {
        case Hothouse::TOGGLESWITCH_UP:     dsp_.setMode(FuzzMode::Fuzz_I);     break;
        case Hothouse::TOGGLESWITCH_MIDDLE: dsp_.setMode(FuzzMode::Fuzz_II);    break;
        case Hothouse::TOGGLESWITCH_DOWN:   dsp_.setMode(FuzzMode::Gain_Boost); break;
        default: break;
    }

    // Update bypass LED state.
    led_bypass_.Set(bypassed_.load(std::memory_order_relaxed) ? 0.0f : 1.0f);
    led_bypass_.Update();
}
void HothouseAdapter::AudioCallback(AudioHandle::InputBuffer in,
                                    AudioHandle::OutputBuffer out,
                                    size_t size) {
    const bool bypass = bypassed_.load(std::memory_order_relaxed);

    for (size_t i = 0; i < size; ++i) {
        if (bypass) {
            // Buffered bypass: dry signal straight through (mono -> dual mono).
            out[0][i] = out[1][i] = in[0][i];
        } else {
            float processed = audioDsp_->processSample(in[0][i]);
            out[0][i] = out[1][i] = processed;
        }
    }
}
