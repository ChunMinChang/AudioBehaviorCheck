#include "AudioStream.h"
#include "utils.h"      // for delay
#include <math.h>       // for M_PI, sin
#include <vector>       // for std::vector
#include <type_traits>  // std::is_same

const double kFequency = 44100.0;
const unsigned int kChannels = 2;

bool gCalled = false;

template<typename T> T ConvertSample(double aInput);
template<> float ConvertSample(double aInput) { return aInput; }
template<> short ConvertSample(double aInput) { return short(aInput * 32767.0f); }

class Synthesizer
{
public:
  Synthesizer(unsigned int aChannels, float aRate, double aVolume = 0.5)
    : mChannels(aChannels)
    , mRate(aRate)
    , mVolume(aVolume)
    , mPhase(std::vector<float>(aChannels, 0.0f))
  {}

  template<typename T>
  void Run(T* aBuffer, long aframes)
  {
    for (unsigned int i = 0; i < mChannels; ++i) {
      float increment = 2.0 * M_PI * GetFrequency(i) / mRate;
      for(long j = 0 ; j < aframes; ++j) {
        aBuffer[j * mChannels + i] = ConvertSample<T>(sin(mPhase[i]) * mVolume);
        mPhase[i] += increment;
      }
    }
  }

private:
  float GetFrequency(int aChannelIndex)
  {
    return 220.0f * (aChannelIndex + 1);
  }

  unsigned int mChannels;
  float mRate;
  double mVolume;
  std::vector<float> mPhase;
};

Synthesizer gSynthesizer(kChannels, kFequency);

/* AudioCallback */
template<typename T>
void callback(void* aBuffer, unsigned long aFrames)
{
  gSynthesizer.Run(static_cast<T*>(aBuffer), aFrames);
  gCalled = true;
}

template<typename T>
void play_sound()
{
  AudioStream::Format format;
  if (std::is_same<T, float>::value) {
    format = AudioStream::F32LE;
  } else if (std::is_same<T, short>::value) {
    format = AudioStream::S16LE;
  } else {
    assert(false && "Unsupport type!");
  }

  AudioStream as(format, kChannels, kFequency, callback<T>);

  as.Start();
  delay(1000);
  as.Stop();

  assert(gCalled && "Callback should be fired!");
}

int main()
{
  play_sound<float>();
  play_sound<short>();
  return 0;
}
