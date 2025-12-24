#include <cstring>
#include <chrono>

#include "audioengine.h"

#include "audioclients/portaudioclient.h"
#include "audioclients/dummyclient.h"

namespace core {

  uint32_t AudioEngine::m_deviceNumber= 0;
  uint32_t AudioEngine::m_sampleRate = 0;
  uint32_t AudioEngine::m_bufferSize = 0;

  void AudioEngine::initialize(uint32_t sampleRate, uint32_t bufferSize)
  {
    m_sampleRate = sampleRate;
    m_bufferSize = bufferSize;
    m_audioClient = std::move(std::make_unique<PortaudioClient>(m_deviceNumber, this));

    Project::get().initialize();
  }

  AudioEngine::~AudioEngine() {
  }

  void AudioEngine::renderBuffer(float* outputBuffer, uint64_t framesPerBuffer) {
    auto& project = Project::get();

    memset(outputBuffer, 0, sizeof(float) * framesPerBuffer * 2);
  }


} // namespace core 
