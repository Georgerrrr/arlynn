#include <cmath>

#include <variant>
#include <algorithm>

#include "core/audioengine.h"
#include "core/signal.h"

#include "oscillator.h"

namespace core {

namespace nodes {

  constexpr auto OSC_SIN = 0;
  constexpr auto OSC_SQUARE = 1;
  constexpr auto OSC_TRIANGLE = 2;
  constexpr auto OSC_SAW = 3;
  constexpr auto MODULATOR_INPUT = 0;
  constexpr auto FREQUENCY_INPUT = 1;

  Oscillator::Oscillator()
   : Node()
   , m_sampleRate(AudioEngine::getSampleRate())
   , m_bufferSize(AudioEngine::getBufferSize())
   , m_modulation(0.5f)
   , m_frequency(440.f)
   , m_waveType(0)
  {
    registerAttribute(&m_modulation);
    registerAttribute(&m_frequency);
    registerAttribute(&m_waveType);
    registerOutput(AudioSignal(Oscillator::render, this));
    registerInput(NodeInput(signal_t::audio));
    registerInput(NodeInput(signal_t::frequency));
  }

  void Oscillator::render(std::vector<float>& output, uint32_t bufferSize, void* userData) {

    Oscillator* osc = reinterpret_cast<Oscillator*>(userData);

    std::fill(output.begin(), output.end(), 0.f);

    const NodeInput& fi = osc->getInput(FREQUENCY_INPUT);
    const NodeInput& mi = osc->getInput(MODULATOR_INPUT);
    float freq = osc->m_frequency;
    float offset = 0;

    for (size_t i = 0 ; i < bufferSize ; i++) {
      if (fi.node != nullptr) {
        freq = std::get<FrequencySignal>(fi.node->getOutput(fi.index)).data.at(i);
      }
      if (mi.node != nullptr) {
        offset = std::get<AudioSignal>(mi.node->getOutput(mi.index)).data.at(i) *
                 osc->m_modulation *
                 freq;
      }
      output[i] = osc->generateFrame(freq, offset);
    }
  }

  float Oscillator::generateFrame(float frequency, float offset) {
    const float N = M_PI * 2.f / m_sampleRate;
    const float phaseDelta = m_frequency + offset;
    const float sinValue = sin(N * m_phaseIndex);
    m_phaseIndex += phaseDelta;

    switch (m_waveType)
    {
      case OSC_SIN:
        return sinValue;
      case OSC_SQUARE:
        return sinValue > 0.f ? 1.f : -1.f;
      case OSC_TRIANGLE:
        return (float)(asin(sinValue) * 2.0 / M_PI);
      case OSC_SAW:
        return -1.f + (2.f * (float)((int)m_phaseIndex % (int)m_sampleRate) / m_sampleRate);
      default:
        return 0.f;
    }
    return sinValue;
  }

} // namespace nodes

} // namespace core
