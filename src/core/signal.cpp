#include "audioengine.h"

#include "signal.h"

namespace core {

  AudioSignal::AudioSignal(render_t render, void* userData)
    : data(AudioEngine::getBufferSize())
    , render(render)
    , userData(userData)
  {}

  AmplificationSignal::AmplificationSignal(render_t renderFunction, void* userData)
    : data(AudioEngine::getBufferSize())
    , render(renderFunction)
    , userData(userData)
  {}

  FrequencySignal::FrequencySignal(render_t renderFunction, void* userData)
    : data(AudioEngine::getBufferSize())
    , render(renderFunction)
    , userData(userData)
  {}

  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  signal_t getOutputFormat(const NodeOutput& output) {
    return std::visit(overloaded{
        [](const AudioSignal& signal) { return signal.signal; },
        [](const AmplificationSignal& signal) { return signal.signal; },
        [](const FrequencySignal& signal) { return signal.signal; }
        }, output);
  }

} // namespace core
