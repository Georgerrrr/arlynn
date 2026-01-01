#include <cstdio>

#include "core/audioengine.h"

void oscinit(core::Node&, uint32_t, uint32_t);
void oscclose();

int main(int argc, char** argv) {
  core::AudioEngine::get().initialize(48000, 128);

  getchar();
  core::AudioEngine::get().addNode(oscinit, oscclose);
  getchar();
  core::AudioEngine::get().sendMessage(core::_set_connection{0, 0, 1, 0});
  getchar();
  core::AudioEngine::get().sendMessage(core::_set_connection{0, 1, 1, 0});
  getchar();

  core::AudioEngine::get().close();

  return 0;
}

