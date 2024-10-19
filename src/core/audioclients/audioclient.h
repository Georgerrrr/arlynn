#pragma once 

#include <cstdint>

namespace core {

  class AudioEngine;

  class AudioClient {
    public:
    AudioClient(uint32_t& deviceNumber, AudioEngine* audioEngine) : m_audioEngine(audioEngine) {}
    virtual ~AudioClient() {}

    protected:
    virtual void callback(float* outputBuffer, uint64_t framesPerBuffer) = 0;

    AudioEngine* m_audioEngine;
  };

} // namespace core 
