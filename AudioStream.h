#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <AudioUnit/AudioUnit.h>

typedef void (* AudioCallback)(void* buffer, unsigned long frames);

class AudioStream
{
public:
  // We only support output for now.
  // enum Side
  // {
  //   OUTPUT,
  //   INPUT
  // }

  enum Format
  {
    S16LE, // PCM signed 16-bit little-endian
    S16BE, // PCM signed 16-bit big-endian
    F32LE, // PCM 32-bit floating-point little-endian
    F32BE  // PCM 32-bit floating-point big-endian
  };

  AudioStream(Format aFormat,
              unsigned int aRate,
              unsigned int aChannels,
              AudioCallback aCallback);

  ~AudioStream();

  void Start();
  void Stop();

private:
  void CreateAudioUnit();
  bool SetDescription(Format aFormat);
  bool SetCallback();
  static OSStatus DataCallback(void* aRefCon,
                               AudioUnitRenderActionFlags* aActionFlags,
                               const AudioTimeStamp* aTimeStamp,
                               UInt32 aBusNumber,
                               UInt32 aNumFrames,
                               AudioBufferList* aData);

  unsigned int mRate;
  unsigned int mChannels;
  AudioStreamBasicDescription mDescription; // Format descriptions
  // AudioUnit is a pointer to ComponentInstanceRecord
  AudioUnit mUnit;
  AudioCallback mCallback;
};

#endif // #ifndef AUDIOSTREAM_H
