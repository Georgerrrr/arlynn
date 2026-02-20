# Arlynn

Arlynn is a cross-platform software synthesiser.
It is built around the concept of modular synthesis.
By connecting nodes which either generate some signal, or affect the signal in a specific way, the user gets the sound that they desire.

## Dependencies

 - Portaudio
 - RtMidi
 - FFTW3

## How to build

```
mkdir build
cd build 
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release 
make install
```

## Todo

 - Add midi support 
 - Add more nodes
 - Add undo
 - Add waveform viewer and frequency viewer
 - Add support for more audio backends not just rely upon portaudio
