#include <iostream>

#include "dummyclient.h"

#include "core/audioengine.h"

const static int channels = 2;

namespace core
{

DummyClient::DummyClient(
    uint32_t& deviceNumber, AudioEngine* audioEngine)
    : AudioClient(deviceNumber, audioEngine)
    , m_running(false)
{
  deviceNumber = 0;

  m_sampleRate = audioEngine->getSampleRate();
  m_bufferSize = audioEngine->getBufferSize();

  std::cout << "Dummy Client Opened With\n";
  std::cout << "Sample Rate: " << m_sampleRate << "\n";
  std::cout << "Buffer Size: " << m_bufferSize << "\n";

  m_buffer = new float[m_bufferSize * channels];
  m_audioThread = std::thread { dummyCallback, this };
  m_running = true;
}

DummyClient::~DummyClient() 
{
  std::cout << "Dummy Client Closed\n";
  m_running = false;
  m_audioThread.join();
  delete[] m_buffer;
}


void DummyClient::dummyCallback(DummyClient* client)
{
  while (client->m_running)
  {
    const auto startTime = std::chrono::high_resolution_clock::now();

    client->callback(client->m_buffer, client->m_bufferSize);

    const auto endTime = std::chrono::high_resolution_clock::now();

    const auto time = (endTime - startTime);
    if (time.count() > 0)
      std::this_thread::sleep_for(time);
  }
}

void DummyClient::callback(float* outputbuffer, unsigned long framesPerBuffer)
{
  m_audioEngine->renderBuffer(outputbuffer, framesPerBuffer);
}

} // namespace core
