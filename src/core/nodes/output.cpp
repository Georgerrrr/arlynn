#include <cstdint>

#include "core/node.h"

void outputinit(core::Node& node, uint32_t sampleRate, uint32_t bufferSize) {
  node.addInput(core::signal_t::audio);
  node.addInput(core::signal_t::audio);
}

void outputclose() {
}

