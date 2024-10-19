#include <algorithm>

#include "amplifier.h"

namespace core {

namespace nodes {

  static constexpr auto GAIN_INPUT = 0;
  static constexpr auto AUDIO_INPUT = 1;

  Amplifier::Amplifier()
    : Node() 
  {
    m_ampControl = 0.5;

    registerInput(NodeInput(signal_t::amplification));
    registerInput(NodeInput(signal_t::audio));
    registerOutput(AudioSignal(Amplifier::render, this));
    registerAttribute(&m_ampControl);
  }

  void Amplifier::render(std::vector<float>& output, uint32_t bufferSize, void* userData) {
    std::fill(output.begin(), output.end(), 0.f);

    Amplifier* amp = reinterpret_cast<Amplifier*>(userData);

    auto gainInput = amp->getInput(GAIN_INPUT);
    auto audioInput = amp->getInput(AUDIO_INPUT);

    if (audioInput.node == nullptr) return;

    AudioSignal audioSource = std::get<AudioSignal>(audioInput.node->getOutput(audioInput.index));

    std::transform(
      output.cbegin(), output.cend(), audioSource.data.cbegin(), output.begin(),
      [amp](float a, float b)
      {
        return (a + b) * static_cast<float>(amp->m_ampControl);
      });

    if (gainInput.node == nullptr) return;

    AmplificationSignal gainSource = std::get<AmplificationSignal>(gainInput.node->getOutput(gainInput.index));

    std::transform(
        output.cbegin(), output.cend(), gainSource.data.cbegin(), output.begin(),
        [] (float a, float b) { return a * b; });
  }

} // namespace nodes

} // namespace core
