#include <cmath>

#include "node.h"

static uint32_t SampleRate;
static double PhaseIndex;

static float generateFrame(float frequency) {
  const float N = M_PI * 2.f / SampleRate;
  const float phaseDelta = frequency;
  const float sinValue = sin(N * PhaseIndex);
  PhaseIndex += phaseDelta;

  return sinValue;
}

static void oscrender(const Node& node, std::vector<float>& data, uint32_t bufferSize) {
  for (auto& sample : data) {
    sample = generateFrame(440) * 0.2;
  }
}

namespace oscillator {

  void init(Node& node, uint32_t sampleRate, uint32_t bufferSize) {
    node.addOutput(audio_signal_t(oscrender, bufferSize));

    SampleRate = sampleRate;
    PhaseIndex = 0.0;
  }

  void close() {
  }

}
