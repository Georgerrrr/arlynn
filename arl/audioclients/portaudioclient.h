#pragma once 

#include <cstdio>

#include <portaudio.h>

#include "audioclient.h"

class PortaudioClient : public AudioClient {
  public:
  PortaudioClient(int32_t& device, render_t renderfunc);
  ~PortaudioClient() override;

  void close() override;

  private:
  static int paCallback(
      const void* inputBuffer, void* outputBuffer,
      unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
      PaStreamCallbackFlags statusFlags, void* userData);

  PaStream* m_stream = nullptr;

  void portaudioError(PaError err) {
    Pa_Terminate();
    fprintf(stderr, "Portaudio Error!\n");
    fprintf(stderr, "Error Number: %d!\n", err);
    fprintf(stderr, "Error Message: %s!\n", Pa_GetErrorText(err));
  }
};

