// HyperF for Hothouse DIY DSP Platform
// Copyright (C) 2024 Jesus Herrera <jesusherrera1594@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// ### Uncomment if IntelliSense can't resolve DaisySP-LGPL classes ###
// #include "daisysp-lgpl.h"

#include "daisysp.h"
#include "hothouse.h"
#include "FuzzProcessor.h"
#include "HotHouseAdapter.h"
#include <memory>

using clevelandmusicco::Hothouse;
using daisy::SaiHandle;

Hothouse hw;
std::unique_ptr<HyperFuzzDSP> fuzzDSP;
std::unique_ptr<HothouseAdapter> adapter;

int main() {
  hw.Init();
  hw.SetAudioBlockSize(48);
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  fuzzDSP = std::make_unique<HyperFuzzDSP>(hw.AudioSampleRate());
  adapter = std::make_unique<HothouseAdapter>(hw, *fuzzDSP);

  hw.StartAdc();
  hw.StartAudio(HothouseAdapter::AudioCallback);

  while (true) {
    // Control-rate updates (knobs/switches -> DSP atomics + filter coeffs).
    adapter->updateControls();
    hw.DelayMs(10); // avoid busy-waiting
    // FOOTSWITCH_1 held 2s -> DFU/bootloader.
    hw.CheckResetToBootloader();
  }
  
  return 0;
}