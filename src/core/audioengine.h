#pragma once 

#include <cstdint>
#include <memory>

#include "project.h"

#include "audioclients/audioclient.h"

namespace core {

  /**
   * Class representing the audio engine
   * 
   *  This class renders the audio buffer based on information given from the project.
   *  This buffer is then passed to the underlying audio library using the AudioClient.
   */
  class AudioEngine {
    public:
    AudioEngine(const AudioEngine&) = delete;
    void operator=(const AudioEngine&) = delete;
    void initialize(uint32_t sampleRate, uint32_t bufferSize); /**< AudioEngine::initialize initializes the AudioClient and the Project */
    ~AudioEngine();

    static AudioEngine& get() {
      static AudioEngine instance;
      return instance;
    }

    static const uint32_t getDeviceNumber() { return m_deviceNumber; } /**< @return m_deviceNumber */
    static const uint32_t getSampleRate() { return m_sampleRate; } /**< @return m_sampleRate */
    static const uint32_t getBufferSize() { return m_bufferSize; } /**< @return m_bufferSize */

    /** 
     * Method to render audio buffer.
     *
     * There is a call to Project::m_outputNode.renderBuffer().
     * This node's 2 outputs (left channel, right channel) are then used to fill the output buffer. 
     *
     * @param outputBuffer is a float array 
     * @param framesPerBuffer represents the size of outputBuffer. It is exactly half the length of the array due to stereo sound
     */
    void renderBuffer(float* outputBuffer, uint64_t framesPerBuffer);

    private:
    AudioEngine() {};
    std::unique_ptr<AudioClient> m_audioClient = nullptr;

    static uint32_t m_deviceNumber;
    static uint32_t m_sampleRate;
    static uint32_t m_bufferSize;
  };

} // namespace core 
