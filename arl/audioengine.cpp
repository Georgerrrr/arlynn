#include <cstring>
#include <chrono>

#include "audioengine.h"

#include "audioclients/audioclient.h"
#include "audioclients/portaudioclient.h"
#include "audioclients/dummyclient.h"

namespace audio {

  static AudioClient* audioclient = nullptr;

  constexpr size_t MaxNodes = 1024;

  static size_t nextid;

  static int32_t devicenumber;
  static uint32_t samplerate;
  static uint32_t buffersize;

  static fifo_t<std::unique_ptr<Node>> addFifo{1};
  static fifo_t<std::unique_ptr<Node>> removeFifo{1};

  static fifo_t<message_t> messages{32};

  static std::vector<std::unique_ptr<Node>> nodes;

  static void renderbuffer(float* buffer, uint64_t framesPerBuffer) noexcept;

  void Init(int32_t dn, uint32_t sr, uint32_t bs) {
    devicenumber = dn;
    samplerate = sr;
    buffersize = bs;

    audioclient = new PortaudioClient(devicenumber, renderbuffer);
  }

  void Close() {
    if (nullptr != audioclient) {
      audioclient->close();
      delete audioclient;
    }

    audioclient = nullptr;
  }

  uint32_t DeviceNumber() { return devicenumber; }
  uint32_t SampleRate() { return samplerate; }
  uint32_t BufferSize() { return buffersize; }

  int16_t AddNode(Node::node_init iFunc, Node::node_close cFunc) {
    auto node = std::make_unique<Node>();
    node->Init = iFunc;
    node->Close = cFunc;

    node->id = nextid;
    nextid++;

    int16_t ret = node->id;

    node->Init(*node, samplerate, buffersize);

    /* todo: make this loop back off */
    while (!addFifo.empty());
    addFifo.push(std::move(node));

    return ret;
  }

  void RemoveNode(int16_t id) {
    SendMessage(_remove_node{id});

    /* wait for the audio thread to put the node that it should 
     * remove onto the remove fifo.
     * todo: make this loop back off */
    while (removeFifo.empty());

    while (!removeFifo.empty()) {
      auto n = removeFifo.pop();
      if (!n) continue;
      /* now that n owns the unique_ptr, is should be deleted here. */
    }
  }

  void UpdateNodeIndex(int16_t& id) {
    if (id < nodes.size()) {
      if (nodes[id]->id == id) return;
    }

    auto it = std::find_if(nodes.begin(), nodes.end(), [&](auto& node) { return node->id == id; });

    if (it == nodes.end()) id = -1;
    id = std::distance(nodes.begin(), it);
  }

  Node& GetNode(int16_t id) {
    return *nodes[id];
  }

  void SendMessage(message_t message) {
    messages.push(message);
  }

  static void processMessages();

  void renderbuffer(float* buffer, uint64_t framesPerBuffer) noexcept {
    memset(buffer, 0, sizeof(float) * framesPerBuffer * 2);

    processMessages();

    if (nodes.size() == 0) return; /* at the start of the app this is fair enough,
                                      since it needs to wait for the output node 
                                      needs to added after init */

    for (auto& node : nodes) {
      node->HasRendered = false;
    }

    if (nodes[0]->getInputs().size() != 2) return; /* this is weird, only the 
                                                      output node should ever be
                                                      at position 0 in nodes */

    nodes[0]->Process();

    for (auto channel = 0 ; channel < 2 ; channel++) {
      auto input = nodes[0]->getInputs()[channel];
      if (input.connection.cachedNodeIndex == -1) continue;

      float* writeptr = buffer + channel;

      auto& c = nodes[input.connection.cachedNodeIndex];
      auto& signal = std::get<audio_signal_t>(c->getOutputs()[input.connection.index]);
      for (auto i = 0 ; i < framesPerBuffer ; i++) {
        *writeptr = signal.data[i];
        writeptr += 2;
      }
    }
  }

  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  static void processMessages() {
    while (!addFifo.empty()) {
      auto newNode = addFifo.pop();
      if (!newNode) continue;
      nodes.push_back(std::move(*newNode));
    }

    while (!messages.empty()) {
        /* process the messages */
        auto message = messages.pop();
        if (!message) continue;

        std::visit(overloaded{
            [&](_remove_node m) { 
              auto r = std::remove_if(nodes.begin(), nodes.end(), [m](auto& node) { return node->id == m.nodeId; });
              removeFifo.push(std::move(*r));
              nodes.erase(r, nodes.end());
            },
            [&](_set_connection m) {
              auto node = std::find_if(nodes.begin(), nodes.end(), [m](auto& node) { return node->id == m.NodeA; });
              (*node)->setInput(m.IndexA, m.NodeB, m.IndexB);
            }
            }, *message);
    }
  }

} // namespace audio 

