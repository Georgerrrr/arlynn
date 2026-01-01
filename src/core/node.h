#pragma once 

#include <cstdint>
#include <functional>
#include <vector>

#include "signal.h"

namespace core {

  struct Node {
    typedef std::function<void(Node&, uint32_t, uint32_t)> node_init;
    typedef std::function<void()> node_close;

    ~Node();

    void addInput(core::signal_t input);
    void addOutput(const node_output_t& output);

    const std::vector<node_input_t>& getInputs() const { return Inputs; }
    const std::vector<node_output_t>& getOutputs() const { return Outputs; }

    void setInput(int16_t input, int16_t node, int16_t index) {
      Inputs[input].connection.nodeId = node;
      Inputs[input].connection.cachedNodeIndex = node;
      Inputs[input].connection.index = index;
    }

    node_init    Init;
    node_close   Close;

    int16_t id;
    bool HasRendered{};

    void Process();

    private:
    
    std::vector<node_input_t> Inputs;
    std::vector<node_output_t> Outputs;
  };

} // namespace core 
