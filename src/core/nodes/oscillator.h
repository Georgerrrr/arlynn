#pragma once 

#include <cstdint>
#include <vector>

#include "../node.h"

namespace core {
  
namespace nodes {

  class Oscillator : public Node {
    public:
    Oscillator();

    float generateFrame(float frequency, float offset, int waveType);

    static void render(std::vector<float>& output, uint32_t bufferSize, void* userData);

    private:
    double m_phaseIndex;
    uint32_t m_sampleRate;
    uint32_t m_bufferSize;

    const size_t MODULATION;
    const size_t FREQUENCY;
    const size_t WAVE_TYPE;
    const size_t MODULATION_INPUT;
    const size_t FREQUENCY_INPUT;
  };

} // namespace nodes

} // namespace core
