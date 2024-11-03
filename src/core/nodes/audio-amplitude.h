#pragma once 

#include <vector>

#include "../node.h"

namespace core {

namespace nodes {

  class AudioToAmplitude : public Node {
    public:
    AudioToAmplitude();

    static void render(std::vector<float>& output, uint32_t bufferSize, void* userData);

    private:
    const size_t CONVERTION_TYPE;
    const size_t MIN_VALUE;
    const size_t AUDIO_INPUT;
  };

} // namespace nodes 

} // namespace core
