#include "dspPrimitives.h"
#include <cmath>

dsp::BiquadFilter::BiquadFilter() = default;

void dsp::BiquadFilter::calculateNotch(float sampleRate, float frequency, float q) { 
    float w0 = 2.0f * M_PI * frequency / sampleRate;
                float alpha = std::sin(w0) / (2.0f * q);
                float cosw0 = std::cos(w0);

                b0 = 1.0f;
                b1 = -2.0f * cosw0;
                b2 = 1.0f;
                a0 = 1.0f + alpha;
                a1 = -2.0f * cosw0;
                a2 = 1.0f - alpha;

                normalize();
}

inline float dsp::BiquadFilter::process(float input) {
    float output = (b0 / a0) * input + (b1 / a0) * z1 + (b2 / a0) * z2
                            - (a1 / a0) * y1 - (a2 / a0) * y2;
                
                // Update state
                z2 = z1;
                z1 = input;
                y2 = y1;
                y1 = output;

                return output;
}

void dsp::BiquadFilter::normalize() {
    // Ensures coefficients are stable
    b0 /= a0; b1 /= a0; b2 /= a0;
    a1 /= a0; a2 /= a0;
}

inline float dsp::FuzzEngine::process(float input, float gain) const {
     // 1. Pre-gain staging
     float amplified = input * gain;
                
     // 2. Octave-up generation via full-wave rectification
     // Flipping the negative half of the cycle upside down creates even-order harmonics
     float rectified = std::abs(amplified); 

     // 3. Asymmetrical Hard Clipping (Silicon diode simulation)
     if (rectified > 0.8f) rectified = 0.8f;
     if (rectified < -0.6f) rectified = -0.6f;

     return rectified;
};