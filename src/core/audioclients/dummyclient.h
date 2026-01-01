#pragma once

#include <thread>

#include "audioclient.h"

namespace core
{

class AudioEngine;

class DummyClient : public AudioClient
{
  public:
  DummyClient(uint32_t& deviceNumber, AudioEngine* audioEngine);

  ~DummyClient() override;

  void close() override;

  private:
  bool m_running;

  unsigned int m_sampleRate;
  unsigned int m_bufferSize;
  float* m_buffer;

  std::thread m_audioThread;

  static void dummyCallback(DummyClient* client);
  void callback(float* outputbuffer, unsigned long framesPerBuffer) override;
};

} // namespace core
