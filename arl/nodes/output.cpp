#include <cstdint>

#include "node.h"

namespace output {

void init(Node& node, uint32_t sampleRate, uint32_t bufferSize) {
  node.addInput(signal_t::audio);
  node.addInput(signal_t::audio);
}

void close() {
}

}
