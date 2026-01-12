#include <iostream>
#include <variant>

#include "audioengine.h"
#include "node.h"

namespace core {

  Node::~Node() {
    if (Close != nullptr)
      this->Close();
  }

  void Node::addInput(signal_t input) {
    Inputs.push_back(node_input_t(input));
  }

  void Node::addOutput(const node_output_t& output) {
    Outputs.push_back(output);
  }

  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  void Node::Process(){
    if (HasRendered) return;

    for (auto& input : Inputs) {
      if (input.connection.nodeId == -1) continue;
      audio::UpdateNodeIndex(input.connection.cachedNodeIndex);
      auto& node = audio::GetNode(input.connection.cachedNodeIndex); 
      node.Process();
    }

    for (auto& output : Outputs) {
      std::visit(overloaded{
          [&](audio_signal_t& signal) { signal.render(*this, signal.data, signal.data.size()); },
          [&](amplification_signal_t& signal) { signal.render(*this, signal.data, signal.data.size()); },
          [&](frequency_signal_t& signal) { signal.render(*this, signal.data, signal.data.size()); }
          }, output);
    }

    HasRendered = true;
  }

} // namespace core 
