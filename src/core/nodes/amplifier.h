#pragma once 

#include "../node.h"

namespace core {

namespace nodes {

  class Amplifier : public Node {
    public:
    Amplifier();

    static void render(std::vector<float>& output, uint32_t bufferSize, void* userData);

    private:
    const size_t GAIN;
    const size_t GAIN_INPUT;
    const size_t AUDIO_INPUT;
  };

} // namespace nodes

} // namespace core
