#include <cmath>

#include "core/node.h"

static uint32_t SampleRate;
static double PhaseIndex;

float generateFrame(float frequency) {
  const float N = M_PI * 2.f / SampleRate;
  const float phaseDelta = frequency;
  const float sinValue = sin(N * PhaseIndex);
  PhaseIndex += phaseDelta;

  return sinValue;
}

void oscrender(const core::Node& node, std::vector<float>& data, uint32_t bufferSize) {
  for (auto& sample : data) {
    sample = generateFrame(440) * 0.2;
  }
}

void oscinit(core::Node& node, uint32_t sampleRate, uint32_t bufferSize) {
  node.addOutput(core::audio_signal_t(oscrender, bufferSize));

  SampleRate = sampleRate;
  PhaseIndex = 0.0;
}

void oscclose() {
}

