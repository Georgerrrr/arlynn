#pragma once 

#include "core/node.h"
#include "core/signal.h"

namespace core {

namespace nodes {

  class Output : public Node {
    public:
    Output()
      : Node()
    { 
      registerInput(NodeInput(signal_t::audio));
      registerInput(NodeInput(signal_t::audio));
    }

    constexpr std::string name() override { return "Output"; }
    constexpr std::string author() override { return "George Ridout"; }
    constexpr std::string version() override { return "1.0.0"; }
  };

} // namespace nodes

} // namespace core
