#pragma once 

#include <vector>

#include "core/node.h"

namespace core {

namespace nodes {

  class AudioToAmplitude : public Node {
    public:
    AudioToAmplitude();

    static void render(std::vector<float>& output, uint32_t bufferSize, void* userData);

    constexpr std::string name() override { return "Audio To Amplitude"; }
    constexpr std::string author() override { return "George Ridout"; }
    constexpr std::string version() override { return "1.0.0"; }

    private:
    int m_convertionType;
    double m_minValue;
  };

} // namespace nodes 

} // namespace core
