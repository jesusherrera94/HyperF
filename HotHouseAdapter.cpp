#include "daisy_seed.h"
#include "daisysp.h"
#include "FuzzProcessor.h"

using namespace daisy;
using namespace daisysp;

// Hardware instance
DaisySeed hardware;

// Smart pointer to manage the DSP engine's lifecycle (RAII)
std::unique_ptr<HyperFuzzDSP> fuzzDSP;

// Audio Callback
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        // Process Left Channel
        float processed_L = fuzzDSP->processSample(in[0][i]);
        out[0][i] = processed_L;

        // Process Right Channel (or mirror Left for Mono)
        out[1][i] = processed_L; 
    }
}

// Helper to map Hothouse ADC values to DSP parameters
void mapHardwareControls() {
    // KNOB_1 -> Level (Mapped 0.0 to 1.0)
    float levelInput = hardware.adc.GetFloat(0); // Assuming ADC 0 is KNOB_1
    fuzzDSP->setLevel(levelInput);

    // KNOB_3 -> Gain (Mapped 1.0 to 50.0 for massive compression)
    float gainInput = hardware.adc.GetFloat(2); // Assuming ADC 2 is KNOB_3
    fuzzDSP->setGain(1.0f + (gainInput * 49.0f)); 

    // SWITCH_1 -> Mode
    // Assuming a 3-way toggle mapped to a specific GPIO reading
    // Simplified logic: Using an arbitrary GPIO read for demonstration
    bool switchUp = hardware.system.GetPin(12); // Example GPIO
    bool switchDown = hardware.system.GetPin(13); // Example GPIO

    if (switchUp) {
        fuzzDSP->setMode(FuzzMode::Fuzz_I);
    } else if (switchDown) {
        fuzzDSP->setMode(FuzzMode::Fuzz_II);
    } else {
        fuzzDSP->setMode(FuzzMode::Gain_Boost);
    }
}

int main(void) {
    // 1. Initialize Hardware
    hardware.Init();
    hardware.SetAudioBlockSize(48); // Standard block size for low latency
    float sampleRate = hardware.AudioSampleRate();

    // 2. Initialize DSP Engine via Smart Pointer
    fuzzDSP = std::make_unique<HyperFuzzDSP>(sampleRate);

    // 3. Start Audio processing
    hardware.StartAudio(AudioCallback);

    // 4. Main hardware loop
    while(1) {
        // Read hardware states and update the DSP atomic variables
        mapHardwareControls();

        // Delay to prevent overwhelming the ADC
        System::Delay(10);
    }
}