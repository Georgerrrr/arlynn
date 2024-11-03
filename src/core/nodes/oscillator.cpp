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

  Oscillator::Oscillator()
   : Node()
   , m_sampleRate(AudioEngine::getSampleRate())
   , m_bufferSize(AudioEngine::getBufferSize())
   , MODULATION(registerAttribute<float>(0.5f))
   , FREQUENCY(registerAttribute<float>(440.f))
   , WAVE_TYPE(registerAttribute<int>(0))
   , MODULATION_INPUT(registerInput(NodeInput(signal_t::audio)))
   , FREQUENCY_INPUT(registerInput(NodeInput(signal_t::frequency)))
  {
    registerOutput(AudioSignal(Oscillator::render, this));
  }

  void Oscillator::render(std::vector<float>& output, uint32_t bufferSize, void* userData) {
    Oscillator* osc = reinterpret_cast<Oscillator*>(userData);

    const float& modulation = std::get<float>(osc->getAttribute(osc->MODULATION));

    std::fill(output.begin(), output.end(), 0.f);

    const NodeInput& fi = osc->getInput(osc->FREQUENCY_INPUT);
    const NodeInput& mi = osc->getInput(osc->MODULATION_INPUT);
    float freq = std::get<float>(osc->getAttribute(osc->FREQUENCY));
    float offset = 0;

    for (size_t i = 0 ; i < bufferSize ; i++) {
      if (fi.node != nullptr) {
        freq = std::get<FrequencySignal>(fi.node->getOutput(fi.index)).data.at(i);
      }
      if (mi.node != nullptr) {
        offset = std::get<AudioSignal>(mi.node->getOutput(mi.index)).data.at(i) *
                 modulation *
                 freq;
      }
      output[i] = osc->generateFrame(freq, offset, std::get<int>(osc->getAttribute(osc->WAVE_TYPE)));
    }
  }

  float Oscillator::generateFrame(float frequency, float offset, int waveType) {
    const float N = M_PI * 2.f / m_sampleRate;
    const float phaseDelta = frequency + offset;
    const float sinValue = sin(N * m_phaseIndex);
    m_phaseIndex += phaseDelta;

    switch (waveType)
    {
      case OSC_SIN:
        return sinValue;
      case OSC_SQUARE:
        return sinValue > 0.f ? 1.f : -1.f;
      case OSC_TRIANGLE:
        return static_cast<float>(asin(sinValue) * 2.0 / M_PI);
      case OSC_SAW:
        return -1.f + (2.f * 
            static_cast<float>(
              static_cast<int>(m_phaseIndex) % static_cast<int>(m_sampleRate))
            / m_sampleRate);
      default:
        return 0.f;
    }
    return sinValue;
  }

} // namespace nodes

} // namespace core
