#include <cstdio>

#include "core/audioengine.h"

int main(int argc, char** argv) {
  core::AudioEngine::get().initialize(48000, 128);

  getchar();
}

