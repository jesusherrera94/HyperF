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

void dsp::BiquadFilter::calculateLowShelf(float sampleRate, float frequency, float gainDb) {
    // A is the linear amplitude derived from the requested dB gain (sqrt because shelf math uses A^2)
    float A     = std::pow(10.0f, gainDb / 40.0f);
    float w0    = 2.0f * M_PI * frequency / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / 2.0f * std::sqrt(2.0f); // Q ~ 0.707, gentle shelf
    float twoSqrtAalpha = 2.0f * std::sqrt(A) * alpha;
    b0 =      A * ((A + 1.0f) - (A - 1.0f) * cosw0 + twoSqrtAalpha);
    b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0);
    b2 =      A * ((A + 1.0f) - (A - 1.0f) * cosw0 - twoSqrtAalpha);
    a0 =          (A + 1.0f) + (A - 1.0f) * cosw0 + twoSqrtAalpha;
    a1 = -2.0f *  ((A - 1.0f) + (A + 1.0f) * cosw0);
    a2 =          (A + 1.0f) + (A - 1.0f) * cosw0 - twoSqrtAalpha;
    normalize();
}

void dsp::BiquadFilter::calculateHighShelf(float sampleRate, float frequency, float gainDb) {
    float A     = std::pow(10.0f, gainDb / 40.0f);
    float w0    = 2.0f * M_PI * frequency / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / 2.0f * std::sqrt(2.0f);
    float twoSqrtAalpha = 2.0f * std::sqrt(A) * alpha;
    b0 =      A * ((A + 1.0f) + (A - 1.0f) * cosw0 + twoSqrtAalpha);
    b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0);
    b2 =      A * ((A + 1.0f) + (A - 1.0f) * cosw0 - twoSqrtAalpha);
    a0 =          (A + 1.0f) - (A - 1.0f) * cosw0 + twoSqrtAalpha;
    a1 = 2.0f *   ((A - 1.0f) - (A + 1.0f) * cosw0);
    a2 =          (A + 1.0f) - (A - 1.0f) * cosw0 - twoSqrtAalpha;
    normalize();
}

float dsp::BiquadFilter::process(float input) {
    float output = b0 * input + b1 * z1 + b2 * z2 - a1 * y1 - a2 * y2;
    z2 = z1; z1 = input;
    y2 = y1; y1 = output;
    return output;
}

void dsp::BiquadFilter::normalize() {
    // Ensures coefficients are stable
    b0 /= a0; b1 /= a0; b2 /= a0;
    a1 /= a0; a2 /= a0;
    a0 = 1.0f;
}

float dsp::FuzzEngine::process(float input, float gain) const {
     // 1. Pre-gain staging
     float amplified = input * gain;
                
     // 2. Octave-up generation via full-wave rectification
     // Flipping the negative half of the cycle upside down creates even-order harmonics
     float rectified = std::abs(amplified); 

     // 3. Asymmetrical Hard Clipping (Silicon diode simulation)
     if (rectified > 0.8f) rectified = 0.8f;
     if (rectified < -0.6f) rectified = -0.6f;

     return rectified;
}

void dsp::DCBlocker::prepare(float sampleRate, float cutoffHz) {
    // Map the desired cutoff to the one-pole's pole position.
    R_  = 1.0f - (2.0f * M_PI * cutoffHz / sampleRate);
    x1_ = 0.0f;
    y1_ = 0.0f;
}
float dsp::DCBlocker::process(float input) {
    // y[n] = x[n] - x[n-1] + R * y[n-1]
    float output = input - x1_ + R_ * y1_;
    x1_ = input;
    y1_ = output;
    return output;
}
