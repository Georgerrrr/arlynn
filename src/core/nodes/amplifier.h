#pragma once 

#include "core/node.h"

namespace core {

namespace nodes {

  class Amplifier : public Node {
    public:
    Amplifier();

    constexpr std::string name() override { return "Amplifier"; }
    constexpr std::string author() override { return "George Ridout"; }
    constexpr std::string version() override { return "1.0.0"; }

    static void render(std::vector<float>& output, uint32_t bufferSize, void* userData);

    private:
    double m_ampControl;
  };

} // namespace nodes

} // namespace core
