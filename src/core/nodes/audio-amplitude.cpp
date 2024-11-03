#include <algorithm>

#include "core/signal.h"

#include "audio-amplitude.h"

namespace core {

namespace nodes {

  AudioToAmplitude::AudioToAmplitude()
    : Node() 
    , CONVERTION_TYPE(registerAttribute<int>(0))
    , MIN_VALUE(registerAttribute<float>(0.5f))
    , AUDIO_INPUT(registerInput(NodeInput(signal_t::audio)))
  {
    registerOutput(AmplificationSignal(AudioToAmplitude::render, this));
  }

  void AudioToAmplitude::render(std::vector<float>& output, uint32_t bufferSize, void *userData) {
    AudioToAmplitude* ata = reinterpret_cast<AudioToAmplitude*>(userData);
    const int& convertionType = std::get<int>(ata->getAttribute(ata->CONVERTION_TYPE));
    const float& minValue = std::get<float>(ata->getAttribute(ata->MIN_VALUE));

    std::fill(output.begin(), output.end(), 0.f);

    const NodeInput& audioInput = ata->getInput(ata->AUDIO_INPUT);
    if (audioInput.node == nullptr) return;

    const AudioSignal& signal = std::get<AudioSignal>(audioInput.node->getOutput(audioInput.index));

    if (convertionType == 0) {
      std::transform(
          output.cbegin(), output.cend(), signal.data.cbegin(), output.begin(),
          [minValue](float, float value)
          {
            return value > minValue ? value : minValue;
          });
      return;
    } 
    std::transform(
        output.cbegin(), output.cend(), signal.data.cbegin(), output.begin(),
        [minValue](float, float value)
        {
          return ((1.f - minValue) * (value + 1.f) / 2.f) + minValue;
        });
  }
} // namespace nodes

} // namespace core
