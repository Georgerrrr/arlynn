#pragma once 

#include <cstdint>
#include <memory>

#include "message.h"
#include "fifo.h"
#include "node.h"

#include "audioclients/audioclient.h"

namespace core {

  namespace audio {

    void Init(int32_t devicenumber, uint32_t samplerate, uint32_t buffersize);
    void Close();

    uint32_t DeviceNumber();
    uint32_t SampleRate();
    uint32_t BufferSize();

    /* Add a node to the system. This function is not wait free */
    void AddNode(Node::node_init iFunc, Node::node_close cFunc);

    /* Remove a node from the system. This function is not wait free */
    void RemoveNode(int16_t id);

    void SendMessage(message_t message);

    void UpdateNodeIndex(int16_t& id);
    Node& GetNode(int16_t id);

  } // namespace audio 

} // namespace core 
