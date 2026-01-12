#include "portaudioclient.h"
#include "core/audioengine.h"

namespace core {


  PortaudioClient::PortaudioClient(int32_t& device, render_t renderfunc)
    : AudioClient(device, renderfunc)
  {
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
      portaudioError(err);
      return;
    }

    auto hostApiInfo = (PaHostApiInfo*)Pa_GetHostApiInfo(Pa_GetDefaultHostApi());

    if (-1 == device) {
      device = hostApiInfo->defaultOutputDevice;
    }
    if (device == paNoDevice)
    {
      fprintf(stderr, "No Device Found!\n");
      return;
    }

    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(device);

    PaStreamParameters outputParameters;
    outputParameters.device = device;
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency =
        Pa_GetDeviceInfo(device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
        &m_stream, NULL, &outputParameters, audio::SampleRate(),
        audio::BufferSize(), paNoFlag,
        paCallback, // User Callback Function
        this);

    if (err != paNoError)
    {
      portaudioError(err);
      return;
    }

    err = Pa_StartStream(m_stream);
    if (err != paNoError)
    {
      portaudioError(err);
      return;
    }
  }

  PortaudioClient::~PortaudioClient() {
    Pa_Terminate();
  }

  void PortaudioClient::close() {
    PaError err = Pa_StopStream(m_stream);
    if (err != paNoError)
      portaudioError(err);
  }

  int PortaudioClient::paCallback(
        const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void* userData) {
    auto ac = static_cast<PortaudioClient*>(userData);
    ac->mRenderFunc(reinterpret_cast<float*>(outputBuffer), framesPerBuffer);
    return paContinue;
  }

} // namespace core
