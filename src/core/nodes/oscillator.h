#pragma once 

#include <cstdint>
#include <vector>

#include "core/node.h"

namespace core {
  
namespace nodes {

  class Oscillator : public Node {
    public:
    Oscillator();

    float generateFrame(float frequency, float offset);

    static void render(std::vector<float>& output, uint32_t bufferSize, void* userData);

    constexpr std::string name() override { return "Oscillator"; }
    constexpr std::string author() override { return "George Ridout"; }
    constexpr std::string version() override { return "1.0.0"; }

    private:
    double m_phaseIndex;
    uint32_t m_sampleRate;
    uint32_t m_bufferSize;

    float m_modulation;
    float m_frequency;
    int m_waveType;
  };

} // namespace nodes

} // namespace core
