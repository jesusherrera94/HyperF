#pragma once

namespace dsp {

    // Simple Biquad Filter implementation for Low Pass, High Pass, Shelving, and Notch
    class BiquadFilter {
        public:
            BiquadFilter();

            // Calculates coefficients for a Notch Filter (Used for Fuzz II 1.2kHz scoop)
            void calculateNotch(float sampleRate, float frequency, float q);
            
            // Low shelf (Bass): boosts/cuts below `frequency` by `gainDb`
            void calculateLowShelf(float sampleRate, float frequency, float gainDb);

            // High shelf (Treble): boosts/cuts above `frequency` by `gainDb`
            void calculateHighShelf(float sampleRate, float frequency, float gainDb);

            // Processes a single sample
            float process(float input);

        private:
            void normalize();

            float a0 = 1.0f, a1 = 0.0f, a2 = 0.0f;
            float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
            float z1 = 0.0f, z2 = 0.0f, y1 = 0.0f, y2 = 0.0f;
    };

    // The Fuzz Engine: Handles clipping and the psychoacoustic octave-up
    class FuzzEngine {
        public:
            // Applies Full-Wave Rectification and Asymmetrical Clipping
            // f(x) = clip(|x * gain|)
            float process(float input, float gain) const;
            float processFuzzII(float input, float gain) const;
    };

    // One-pole DC blocker (high-pass ~10-20 Hz).
    // Removes the DC offset introduced by full-wave rectification in FuzzEngine.
    class DCBlocker {
        public:
            DCBlocker() = default;
            // cutoffHz ~ 20 Hz strips DC/subsonics without thinning the low end
            void prepare(float sampleRate, float cutoffHz = 20.0f);
            float process(float input);
        private:
            float R_  = 0.997f;  // pole position (closer to 1.0 => lower cutoff)
            float x1_ = 0.0f;    // previous input  x[n-1]
            float y1_ = 0.0f;    // previous output y[n-1]
    };

} // namespace dsp