#include "portaudioclient.h"
#include "core/audioengine.h"

namespace core {

  PortaudioClient::PortaudioClient(
      uint32_t& deviceNumber, AudioEngine* audioEngine)
    : AudioClient(deviceNumber, audioEngine)
  {
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
      portaudioError(err);
      return;
    }

    auto hostApiInfo = (PaHostApiInfo*)Pa_GetHostApiInfo(Pa_GetDefaultHostApi());
    for (int i = 0; i < Pa_GetHostApiCount(); i++)
    {
      if (Pa_GetHostApiInfo(i)->type == paJACK)
        hostApiInfo = (PaHostApiInfo*)Pa_GetHostApiInfo(i);
    }

    int device = hostApiInfo->defaultOutputDevice;
    deviceNumber = device;
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
        &m_stream, NULL, &outputParameters, audioEngine->getSampleRate(),
        audioEngine->getBufferSize(), paNoFlag,
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
    PaError err = Pa_StopStream(m_stream);
    if (err != paNoError)
      portaudioError(err);
    Pa_Terminate();
  }

  void PortaudioClient::callback(float* outputBuffer, uint64_t framesPerBuffer) {
    m_audioEngine->renderBuffer(outputBuffer, framesPerBuffer);
  }

  int PortaudioClient::paCallback(
        const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void* userData) {
    auto ac = static_cast<PortaudioClient*>(userData);
    ac->callback(reinterpret_cast<float*>(outputBuffer), framesPerBuffer);
    return paContinue;
  }

} // namespace core
