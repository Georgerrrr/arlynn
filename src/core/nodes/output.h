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
  };

} // namespace nodes

} // namespace core
