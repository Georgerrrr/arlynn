#include <algorithm>

#include "core/signal.h"

#include "audio-amplitude.h"

namespace core {

namespace nodes {

  constexpr auto AUDIO_INPUT = 0;

  AudioToAmplitude::AudioToAmplitude()
    : Node() 
    , m_convertionType(0)
    , m_minValue(0.5)
  {
    registerAttribute(&m_convertionType);
    registerAttribute(&m_minValue);
    registerOutput(AmplificationSignal(AudioToAmplitude::render, this));
    registerInput(NodeInput(signal_t::audio));
  }

  void AudioToAmplitude::render(std::vector<float>& output, uint32_t bufferSize, void *userData) {
    AudioToAmplitude* ata = reinterpret_cast<AudioToAmplitude*>(userData);

    std::fill(output.begin(), output.end(), 0.f);

    const NodeInput& audioInput = ata->getInput(AUDIO_INPUT);
    if (audioInput.node == nullptr) return;

    const AudioSignal& signal = std::get<AudioSignal>(audioInput.node->getOutput(audioInput.index));

    if (ata->m_convertionType == 0) {
      std::transform(
          output.cbegin(), output.cend(), signal.data.cbegin(), output.begin(),
          [ata](float, float value)
          {
            float v = static_cast<float>(ata->m_minValue);
            return value > v ? value : v;
          });
      return;
    } 
    std::transform(
        output.cbegin(), output.cend(), signal.data.cbegin(), output.begin(),
        [ata](float, float value)
        {
          float v = static_cast<float>(ata->m_minValue);
          return ((1.f - v) * (value + 1.f) / 2.f) + v;
        });
  }
} // namespace nodes

} // namespace core
