#pragma once 

#include <cstdint>
#include <functional>

typedef void(*render_t)(float*, uint64_t) noexcept;

class AudioClient {
  public:
  /* pass in -1 to set device to default device */
  AudioClient(int32_t& device, render_t renderfunc)
    : mRenderFunc(renderfunc)
  {}
  virtual ~AudioClient() {}

  virtual void close() {}

  render_t mRenderFunc;
};

