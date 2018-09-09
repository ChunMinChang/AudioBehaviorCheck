#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <AudioUnit/AudioUnit.h>

typedef void (* AudioCallback)(void* buffer, unsigned long frames);

class AudioStream
{
public:
  // We only support output for now.
  // enum Scope
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

  struct Parameters
  {
    Format mFormat;
    UInt32 mChannels;
    Float64 mRate;

    AudioStreamBasicDescription GetFormatDescription();
    size_t GetFormatByteSize();
    AudioFormatFlags GetFormatFlags();
  };

  AudioStream(Format aFormat,
              unsigned int aChannels,
              double aRate,
              AudioCallback aCallback);

  ~AudioStream();

  bool Start();
  bool Stop();

private:
  enum Element
  {
    OutputBus = 0,
    InputBus = 1
  };

  bool CreateAudioUnit();
  bool DestroyAudioUnit();
  bool InitAudioUnit();
  bool UninitAudioUnit();
  bool SetStreamFormat();
  bool SetCallback();
  // Render the callback from underlying OS to the callback passed to the stream.
  OSStatus Render(AudioUnitRenderActionFlags* aActionFlags,
                  const AudioTimeStamp* aTimeStamp,
                  UInt32 aBusNumber,
                  UInt32 aNumFrames,
                  AudioBufferList* aData);
  // The static function registered as the audio data callback.
  // It will be fired by underlying OS and come with a buffer to be filled.
  // The `aRefCon` will be assigned to a AudioStream so that the callback knows
  // which stream it belongs.
  static OSStatus DataCallback(void* aRefCon,
                               AudioUnitRenderActionFlags* aActionFlags,
                               const AudioTimeStamp* aTimeStamp,
                               UInt32 aBusNumber,
                               UInt32 aNumFrames,
                               AudioBufferList* aData);

  AudioUnit mUnit;
  AudioCallback mCallback;
  Parameters mParams;
};

#endif // AUDIOSTREAM_H
