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
    using std::chrono::high_resolution_clock;
    auto t1 = high_resolution_clock::now();
    auto& project = Project::get();

    memset(outputBuffer, 0, sizeof(float) * framesPerBuffer * 2);

    if (!project.dataMutex.try_lock())
      return;

    for (auto& node : project.m_nodes) {
      node->setHasRendered(false);
    }

    if (!project.m_outputNode.expired()) {
      const auto outputNode = project.m_outputNode.lock();
      outputNode->renderBuffer();
      const NodeInput& left = outputNode->getInput(0);
      const NodeInput& right = outputNode->getInput(1);

      if (left.node != nullptr) {
        const AudioSignal& leftSignal = std::get<AudioSignal>(left.node->getOutput(left.index));
        for (size_t i = 0 ; i < framesPerBuffer ; i++) {
          outputBuffer[2 * i] = leftSignal.data.at(i);
        }
      }
      if (right.node != nullptr) {
        const AudioSignal& rightSignal = std::get<AudioSignal>(right.node->getOutput(right.index));
        for (size_t i = 0 ; i < framesPerBuffer ; i++) {
          outputBuffer[2 * i + 1] = rightSignal.data.at(i);
        }
      }
    }

    project.dataMutex.unlock();

    auto t2 = high_resolution_clock::now();
    auto bufferTime = duration_cast<std::chrono::milliseconds>(t2 - t1);

  }


} // namespace core 
