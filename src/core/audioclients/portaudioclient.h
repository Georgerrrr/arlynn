#pragma once 

#include <cstdio>

#include <portaudio.h>

#include "audioclient.h"

namespace core {

  class PortaudioClient : public AudioClient {
    public:
    PortaudioClient(uint32_t& deviceNumber, AudioEngine* audioEngine);
    ~PortaudioClient() override;

    private:
    static int paCallback(
        const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void* userData);

    void callback(float* outputBuffer, uint64_t framesPerBuffer) override;

    PaStream* m_stream = nullptr;

    void portaudioError(PaError err) {
      Pa_Terminate();
      fprintf(stderr, "Portaudio Error!\n");
      fprintf(stderr, "Error Number: %d!\n", err);
      fprintf(stderr, "Error Message: %s!\n", Pa_GetErrorText(err));
    }
  };

} // namespace core
