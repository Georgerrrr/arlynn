#pragma once 

#include <cstdint>
#include <memory>

#include "message.h"
#include "fifo.h"
#include "node.h"

#include "audioclients/audioclient.h"

namespace core {

  /**
   * Class representing the audio engine
   * 
   *  This class renders the audio buffer based on information given from the project.
   *  This buffer is then passed to the underlying audio library using the AudioClient.
   */
  class AudioEngine {

    static constexpr auto MaxNodes = 1024;

    public:
    AudioEngine(const AudioEngine&) = delete;
    void operator=(const AudioEngine&) = delete;
    void initialize(uint32_t sampleRate, uint32_t bufferSize); /**< AudioEngine::initialize initializes the AudioClient and the Project */

    void close();

    static AudioEngine& get() {
      static AudioEngine instance;
      return instance;
    }

    static const uint32_t getDeviceNumber() { return m_deviceNumber; } /**< @return m_deviceNumber */
    static const uint32_t getSampleRate() { return m_sampleRate; } /**< @return m_sampleRate */
    static const uint32_t getBufferSize() { return m_bufferSize; } /**< @return m_bufferSize */

    void addNode(Node::node_init iFunc, Node::node_close cFunc);
    std::unique_ptr<Node> removeNode(int16_t id);

    void UpdateNodeIndex(int16_t& id);

    /* send a message to the audio thread */
    void sendMessage(message_t message);

    /** 
     * Method to render audio buffer.
     *
     * @param outputBuffer is a float array 
     * @param framesPerBuffer represents the size of outputBuffer. It is exactly half the length of the array due to stereo sound
     */
    void renderBuffer(float* outputBuffer, uint64_t framesPerBuffer);

    private:
    AudioEngine() 
      : m_audioClient(nullptr)
      , mNodes() 
      , mNewNode(1)
      , mRemoveNode(1)
      , mMessages(32)
    {
      mNodes.reserve(MaxNodes);
    };

    void processMessages();

    std::unique_ptr<AudioClient> m_audioClient;

    std::vector<std::unique_ptr<Node>> mNodes;

    int16_t mNewId{};
    fifo_t<std::unique_ptr<Node>> mNewNode;
    fifo_t<std::unique_ptr<Node>> mRemoveNode;

    fifo_t<message_t> mMessages; /* messages from main thread to audio thread */

    static uint32_t m_deviceNumber;
    static uint32_t m_sampleRate;
    static uint32_t m_bufferSize;

    friend Node;
  };

} // namespace core 
