#pragma once

#include <thread>

#include "audioclient.h"

namespace core
{

class DummyClient : public AudioClient
{
  public:
  DummyClient(int32_t& device, render_t renderfunc);

  ~DummyClient() override;

  void close() override;

  private:
  bool m_running;

  unsigned int m_sampleRate;
  unsigned int m_bufferSize;
  float* m_buffer;

  std::thread m_audioThread;

  static void dummyCallback(DummyClient* client);
};

} // namespace core
