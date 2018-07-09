#include <assert.h>
#include <CoreAudio/CoreAudio.h>

#include "AudioStream.h"

#define AU_OUT_BUS  0
// #define AU_IN_BUS   1

AudioStream::AudioStream(Format aFormat,
                         unsigned int aRate,
                         unsigned int aChannels,
                         AudioCallback aCallback)
  : mRate(aRate)
  , mChannels(aChannels)
  , mUnit(nullptr)
  , mCallback(aCallback)
{
  assert(mRate && mChannels);
  CreateAudioUnit(); // Initialize mUnit
  assert(SetDescription(aFormat)); // Initialize mDescription
  assert(SetCallback()); // Render output to DataCallback
  assert(AudioUnitInitialize(mUnit) == noErr);
}

AudioStream::~AudioStream()
{
  assert(mUnit);
  assert(AudioOutputUnitStop(mUnit) == noErr);
  assert(AudioUnitUninitialize(mUnit) == noErr);
  assert(AudioComponentInstanceDispose(mUnit) == noErr);
}

void
AudioStream::Start()
{
  assert(mUnit);
  assert(AudioOutputUnitStart(mUnit) == noErr);
}

void
AudioStream::Stop()
{
  assert(mUnit);
  assert(AudioOutputUnitStop(mUnit) == noErr);
}

void
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
  assert(comp); // comp will be nullptr if there is no matching audio hardware.

  assert(AudioComponentInstanceNew(comp, &mUnit) == noErr);
  assert(mUnit); // mUnit should NOT be nullptr after initializing.
}

bool
AudioStream::SetDescription(Format aFormat)
{
  memset(&mDescription, 0, sizeof(mDescription));
  switch (aFormat) {
    case S16LE:
      mDescription.mBitsPerChannel = 16;
      mDescription.mFormatFlags = kAudioFormatFlagIsSignedInteger;
      break;
    case S16BE:
      mDescription.mBitsPerChannel = 16;
      mDescription.mFormatFlags = kAudioFormatFlagIsSignedInteger |
                                  kAudioFormatFlagIsBigEndian;
      break;
    case F32LE:
      mDescription.mBitsPerChannel = 32;
      mDescription.mFormatFlags = kAudioFormatFlagIsFloat;
      break;
    case F32BE:
      mDescription.mBitsPerChannel = 32;
      mDescription.mFormatFlags = kAudioFormatFlagIsFloat |
                                  kAudioFormatFlagIsBigEndian;
      break;
    default:
      return false;
  }

  // The mFormatFlags below should be set by "|" or operator,
  // or the assigned flags above will be cleared.
  mDescription.mFormatID = kAudioFormatLinearPCM;
  mDescription.mFormatFlags |= kLinearPCMFormatFlagIsPacked;
  mDescription.mSampleRate = mRate;
  mDescription.mChannelsPerFrame = mChannels;

  mDescription.mBytesPerFrame = (mDescription.mBitsPerChannel / 8) *
                                mDescription.mChannelsPerFrame;

  mDescription.mFramesPerPacket = 1;
  mDescription.mBytesPerPacket = mDescription.mBytesPerFrame *
                                 mDescription.mFramesPerPacket;
  mDescription.mReserved = 0;

  return AudioUnitSetProperty(mUnit,
                              kAudioUnitProperty_StreamFormat,
                              kAudioUnitScope_Input,
                              AU_OUT_BUS,
                              &mDescription,
                              sizeof(mDescription)) == noErr;
}

bool
AudioStream::SetCallback()
{
  AURenderCallbackStruct aurcbs;
  memset(&aurcbs, 0, sizeof(aurcbs));
  aurcbs.inputProc = DataCallback;
  aurcbs.inputProcRefCon = this; // Pass this as callback's arguments

  return AudioUnitSetProperty(mUnit,
                              kAudioUnitProperty_SetRenderCallback,
                              kAudioUnitScope_Global,
                              AU_OUT_BUS,
                              &aurcbs,
                              sizeof(aurcbs)) == noErr;
}

/* static */ OSStatus
AudioStream::DataCallback(void* aRefCon,
                          AudioUnitRenderActionFlags* aActionFlags,
                          const AudioTimeStamp* aTimeStamp,
                          UInt32 aBusNumber,
                          UInt32 aNumFrames,
                          AudioBufferList* aData)
{
  assert(aBusNumber == AU_OUT_BUS);
  assert(aData->mNumberBuffers == 1);

  AudioStream* as = static_cast<AudioStream*>(aRefCon); // Get arguments
  void* buffer = aData->mBuffers[0].mData;
  as->mCallback(buffer, aNumFrames);
  return noErr;
}
