#include "HotHouseAdapter.h"

using clevelandmusicco::Hothouse;
using daisy::AudioHandle;
HyperFuzzDSP* HothouseAdapter::audioDsp_ = nullptr;
HothouseAdapter::HothouseAdapter(Hothouse& hw, HyperFuzzDSP& dsp)
    : hw_(hw), dsp_(dsp) {
    // Bind the instance for the static audio callback.
    audioDsp_ = &dsp;
}
void HothouseAdapter::updateControls() {
    // Refresh ADC + switch readings before mapping.
    hw_.ProcessAllControls();
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
}
void HothouseAdapter::AudioCallback(AudioHandle::InputBuffer in,
                                    AudioHandle::OutputBuffer out,
                                    size_t size) {
    for (size_t i = 0; i < size; ++i) {
        float processed = audioDsp_->processSample(in[0][i]);
        out[0][i] = processed;  // Left
        out[1][i] = processed;  // Right (mono -> dual mono)
    }
}
