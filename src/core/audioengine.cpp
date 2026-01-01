#include <cstring>
#include <chrono>

#include "audioengine.h"

#include "audioclients/portaudioclient.h"
#include "audioclients/dummyclient.h"

void outputinit(core::Node&, uint32_t, uint32_t);
void outputclose();

namespace core {

  uint32_t AudioEngine::m_deviceNumber= 0;
  uint32_t AudioEngine::m_sampleRate = 0;
  uint32_t AudioEngine::m_bufferSize = 0;

  void AudioEngine::initialize(uint32_t sampleRate, uint32_t bufferSize)
  {
    addNode(outputinit, outputclose);

    m_sampleRate = sampleRate;
    m_bufferSize = bufferSize;
    m_audioClient = std::move(std::make_unique<PortaudioClient>(m_deviceNumber, this));
  }

  void AudioEngine::close() {
    m_audioClient->close();
  }

  void AudioEngine::addNode(Node::node_init iFunc, Node::node_close cFunc) {
    auto node = std::make_unique<Node>();
    node->Init = iFunc;
    node->Close = cFunc;

    node->id = mNewId;
    mNewId++;

    node->Init(*node, getSampleRate(), getBufferSize());

    while (!mNewNode.empty());
    mNewNode.push(std::move(node));
  }

  std::unique_ptr<Node> AudioEngine::removeNode(int16_t id) {
    sendMessage(_remove_node{id});

    /* wait for the audio thread to put the removed node 
     * onto the remove node fifo */
    while (mRemoveNode.empty());

    while (!mRemoveNode.empty()) {
      auto n = mRemoveNode.pop();
      if (!n) continue;
      /* should get deleted here */
    }

    return nullptr;
  }

  void AudioEngine::UpdateNodeIndex(int16_t& id) {
    if (id < mNodes.size()) {
      if (mNodes[id]->id == id) return;
    }

    auto it = std::find_if(mNodes.begin(), mNodes.end(), [&](auto& node) { return node->id == id; });

    if (it == mNodes.end()) id = -1;
    id = std::distance(mNodes.begin(), it);
  }

  void AudioEngine::sendMessage(message_t message) {
    mMessages.push(message);
  }

  void AudioEngine::renderBuffer(float* outputBuffer, uint64_t framesPerBuffer) {
    memset(outputBuffer, 0, sizeof(float) * framesPerBuffer * 2);

    processMessages();

    if (mNodes.size() == 0) return; /* tf has happend */

    for (auto& node : mNodes) {
      node->HasRendered = false;
    }

    if (mNodes[0]->getInputs().size() != 2) return; /* ...again... what? */

    mNodes[0]->Process();

    for (auto channel = 0 ; channel < 2 ; channel++) {
      auto input = mNodes[0]->getInputs()[channel];
      if (input.connection.cachedNodeIndex == -1) continue;

      float* writeptr = outputBuffer + channel;

      auto& c = mNodes[input.connection.cachedNodeIndex];
      auto& signal = std::get<audio_signal_t>(c->getOutputs()[input.connection.index]);
      for (auto i = 0 ; i < framesPerBuffer ; i++) {
        *writeptr = signal.data[i];
        writeptr += 2;
      }
    }
  }

  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  void AudioEngine::processMessages() {
    while (!mNewNode.empty()) {
      auto newNode = mNewNode.pop();
      if (!newNode) continue;
      mNodes.push_back(std::move(*newNode));
    }

    while (!mMessages.empty()) {
      /* process the messages */
      auto message = mMessages.pop();
      if (!message) continue;

      std::visit(overloaded{
          [&](_remove_node m) { 
            auto r = std::remove_if(mNodes.begin(), mNodes.end(), [m](auto& node) { return node->id == m.nodeId; });
            mRemoveNode.push(std::move(*r));
            mNodes.erase(r, mNodes.end());
          },
          [&](_set_connection m) {
            auto node = std::find_if(mNodes.begin(), mNodes.end(), [m](auto& node) { return node->id == m.NodeA; });
            (*node)->setInput(m.IndexA, m.NodeB, m.IndexB);
          }
          }, *message);
    }
  }

} // namespace core 
