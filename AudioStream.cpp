#include "AudioStream.h"
#include <CoreAudio/CoreAudio.h>
#include <cassert>

const unsigned int FORMAT_LEN = AudioStream::F32BE + 1;

AudioStreamBasicDescription
AudioStream::Parameters::GetFormatDescription()
{
  AudioStreamBasicDescription desc;
  memset(&desc, 0, sizeof(desc));
  desc.mSampleRate = mRate;
  desc.mChannelsPerFrame = mChannels;
  desc.mFormatID = kAudioFormatLinearPCM;
  desc.mFormatFlags = GetFormatFlags();
  desc.mFramesPerPacket = 1;
  desc.mBytesPerFrame = GetFormatByteSize() * desc.mChannelsPerFrame;
  desc.mBytesPerPacket = desc.mBytesPerFrame * desc.mFramesPerPacket;
  desc.mBitsPerChannel = GetFormatByteSize() * 8;
  desc.mReserved = 0;

  return desc;
}

size_t
AudioStream::Parameters::GetFormatByteSize()
{
  static size_t formatSizes[FORMAT_LEN] = {
    sizeof(short), // S16LE
    sizeof(short), // S16BE
    sizeof(float), // F32LE
    sizeof(float)  // F32BE
  };

  return formatSizes[mFormat];
}

AudioFormatFlags
AudioStream::Parameters::GetFormatFlags()
{
  static AudioFormatFlags formatFlags[FORMAT_LEN] = {
    kAudioFormatFlagIsSignedInteger,                                // S16LE
    kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian,  // S16BE
    kAudioFormatFlagIsFloat,                                        // F32LE
    kAudioFormatFlagIsFloat | kAudioFormatFlagIsBigEndian,          // F32BE
  };

  AudioFormatFlags defaultFlags = kLinearPCMFormatFlagIsPacked;
  return defaultFlags | formatFlags[mFormat];
}

AudioStream::AudioStream(Format aFormat,
                         unsigned int aChannels,
                         double aRate,
                         AudioCallback aCallback)
  : mUnit(nullptr)
  , mCallback(aCallback)
  , mParams({ aFormat,
              static_cast<UInt32>(aChannels),
              static_cast<Float64>(aRate) })
{
  assert(CreateAudioUnit());
  assert(SetStreamFormat());
  assert(SetCallback());
  assert(InitAudioUnit());
}

AudioStream::~AudioStream()
{
  Stop();
  assert(UninitAudioUnit());
  assert(DestroyAudioUnit());
}

bool
AudioStream::Start()
{
  assert(mUnit);
  return AudioOutputUnitStart(mUnit) == noErr;
}

bool
AudioStream::Stop()
{
  assert(mUnit);
  return AudioOutputUnitStop(mUnit) == noErr;
}

bool
AudioStream::CreateAudioUnit()
{
  assert(!mUnit); // mUnit should be nullptr before initializing.

  AudioComponentDescription desc;
  desc.componentType = kAudioUnitType_Output;
  desc.componentSubType = kAudioUnitSubType_DefaultOutput;
  desc.componentManufacturer = kAudioUnitManufacturer_Apple;
  desc.componentFlags = 0;
  desc.componentFlagsMask = 0;

  AudioComponent comp = AudioComponentFindNext(NULL, &desc);
  // comp will be nullptr if there is no matching audio hardware.

  return comp && AudioComponentInstanceNew(comp, &mUnit) == noErr;
}

bool
AudioStream::DestroyAudioUnit()
{
  assert(mUnit);
  return AudioComponentInstanceDispose(mUnit) == noErr;
}

bool
AudioStream::InitAudioUnit()
{
  assert(mUnit);
  return AudioUnitInitialize(mUnit) == noErr;
}

bool
AudioStream::UninitAudioUnit()
{
  assert(mUnit);
  return AudioUnitUninitialize(mUnit) == noErr;
}

bool
AudioStream::SetStreamFormat()
{
  AudioStreamBasicDescription desc = mParams.GetFormatDescription();
  return AudioUnitSetProperty(mUnit,
                              kAudioUnitProperty_StreamFormat,
                              kAudioUnitScope_Input,
                              OutputBus,
                              &desc,
                              sizeof(desc)) == noErr;
}

bool
AudioStream::SetCallback()
{
  AURenderCallbackStruct aurcbs;
  memset(&aurcbs, 0, sizeof(aurcbs));
  aurcbs.inputProc = DataCallback;
  aurcbs.inputProcRefCon = this; // Set the callback target to `this`.

  return AudioUnitSetProperty(mUnit,
                              kAudioUnitProperty_SetRenderCallback,
                              kAudioUnitScope_Input,
                              OutputBus,
                              &aurcbs,
                              sizeof(aurcbs)) == noErr;
}

OSStatus
AudioStream::Render(AudioUnitRenderActionFlags* aActionFlags,
                    const AudioTimeStamp* aTimeStamp,
                    UInt32 aBusNumber,
                    UInt32 aNumFrames,
                    AudioBufferList* aData)
{
  assert(aBusNumber == OutputBus);
  assert(aData->mNumberBuffers == 1);

  void* buffer = aData->mBuffers[0].mData;
  mCallback(buffer, aNumFrames);
  return noErr;
}

/* static */ OSStatus
AudioStream::DataCallback(void* aRefCon,
                          AudioUnitRenderActionFlags* aActionFlags,
                          const AudioTimeStamp* aTimeStamp,
                          UInt32 aBusNumber,
                          UInt32 aNumFrames,
                          AudioBufferList* aData)
{
  assert(aBusNumber == OutputBus);
  assert(aData->mNumberBuffers == 1);

  AudioStream* as = static_cast<AudioStream*>(aRefCon);
  return as->Render(aActionFlags, aTimeStamp, aBusNumber, aNumFrames, aData);
}
