#pragma once 

#include <cstdint>
#include <variant>
#include <vector>
#include <memory>
#include <functional>

namespace core {

  enum class signal_t {
    audio,
    amplification,
    frequency
  };

  /*
   * A signal representing audio data for one buffer 
   */
  struct AudioSignal {
    typedef std::function<void(std::vector<float>&, uint32_t, void*)> render_t;
    static constexpr signal_t signal = signal_t::audio;

    std::vector<float> data;
    render_t render;
    void* userData;

    AudioSignal(render_t render, void* userData);
  };

  /*
   * A signal representing amplification data for one buffer 
   */
  struct AmplificationSignal {
    typedef std::function<void(std::vector<float>&, uint32_t, void*)> render_t;
    static constexpr signal_t signal = signal_t::amplification;

    std::vector<float> data;
    render_t render;
    void* userData;

    AmplificationSignal(render_t renderFunction, void* userData);
  };

  struct FrequencySignal {
    typedef std::function<void(std::vector<float>&, uint32_t, void*)> render_t;
    static constexpr signal_t signal = signal_t::frequency;

    std::vector<float> data;
    render_t render;
    void* userData;

    FrequencySignal(render_t renderFunction, void* userData);
  };

  typedef std::variant<AudioSignal, AmplificationSignal, FrequencySignal> NodeOutput;

  signal_t getOutputFormat(const NodeOutput& output);

  class Node;

  /*
   * A class representing the input of a node 
   *
   * The data inside NodeInput represents a connection to another output of a Node. It also has a format to ensure that connections to outputs are valid.
   */
  struct NodeInput {
    signal_t inputType;
    std::shared_ptr<Node> node;
    size_t index;

    NodeInput(signal_t input)
      : inputType(input)
      , node(nullptr)
      , index(0)
    {}
  };

} // namespace core 
