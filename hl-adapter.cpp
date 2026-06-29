#include "FuzzProcessor.h"

extern "C" {
    void* hl_create(float sampleRate) { 
        // TODO: add sample rate to the constructor
        return new HyperFuzzDSP(sampleRate); 
    }
    void  hl_destroy(void* p) {
        // TODO: add sample rate to the destructor
        delete static_cast<HyperFuzzDSP*>(p); 
    }
    void  hl_process(void* p, float* s, size_t n, uint16_t ch, uint32_t sr) {
        // TODO: Implement the process function
        auto* dsp = static_cast<HyperFuzzDSP*>(p);
        for (size_t i = 0; i < n; ++i) s[i] = dsp->processSample(s[i]);
    }
    void  hl_set_knob(void* p, uint32_t idx, float v) {
        // TODO: Call the set knob functions here!
        auto* dsp = static_cast<HyperFuzzDSP*>(p);
        switch(idx) {
            case 0: dsp->setLevel(v);  break;
            case 1: dsp->setBass(v);   break;
            case 2: dsp->setTreble(v); break;
            case 3: dsp->setGain(1.0f + v * 49.0f); break;
        }
    }
    void  hl_set_switch(void* p, uint32_t idx, int32_t pos) {
        // TODO: Call the set switch functions here!
        if (idx == 0) {
            FuzzMode m = (pos == 0) ? FuzzMode::Fuzz_I
                       : (pos == 1) ? FuzzMode::Fuzz_II
                       :              FuzzMode::Gain_Boost;
            static_cast<HyperFuzzDSP*>(p)->setMode(m);
        }
    }
    void  hl_set_footswitch(void* p, uint32_t idx, bool pressed) { /* bypass handled in HeadroomLab if you need to add function to the other button, add it here! */ }
}