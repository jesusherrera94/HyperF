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
    };

} // namespace dsp