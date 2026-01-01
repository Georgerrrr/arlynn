#pragma once 

#include <cstdint>
#include <functional>
#include <vector>
#include <variant>

namespace core {

  enum class signal_t {
    audio,
    amplification,
    frequency
  };

  struct Node;

  struct audio_signal_t {
    static constexpr signal_t Type = signal_t::audio;

    typedef std::function<void(const Node&, std::vector<float>&, uint32_t)> render_t;

    std::vector<float> data;
    render_t render;

    audio_signal_t(render_t render, uint32_t bufferSize)
      : data(bufferSize)
      , render(render)
    {}
  };

  struct amplification_signal_t{
    static constexpr signal_t Type = signal_t::amplification;

    typedef std::function<void(const Node&, std::vector<float>&, uint32_t)> render_t;

    std::vector<float> data;
    render_t render;

    amplification_signal_t(render_t render, uint32_t bufferSize)
      : data(bufferSize)
      , render(render)
    {}
  };

  struct frequency_signal_t {
    static constexpr signal_t Type = signal_t::frequency;

    typedef std::function<void(const Node&, std::vector<float>&, uint32_t)> render_t;

    std::vector<float> data;
    render_t render;

    frequency_signal_t(render_t render, uint32_t bufferSize)
      : data(bufferSize)
      , render(render)
    {}
  };

  typedef std::variant<audio_signal_t, amplification_signal_t, frequency_signal_t> node_output_t;

  struct _connection {
    int16_t nodeId;
    int16_t cachedNodeIndex;
    int16_t index;
  };

  struct node_input_t {

    signal_t signalType;
    _connection connection;

    node_input_t(signal_t Type)
      : signalType(Type)
      , connection({-1, -1, -1}) 
      {}
  };

} // namespace core
