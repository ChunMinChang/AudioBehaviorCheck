#ifndef AUDIODEVICEUTILS_H
#define AUDIODEVICEUTILS_H

#include <CoreAudio/CoreAudio.h>

#include <string>
#include <vector>

using std::vector;
using std::string;

class AudioDeviceUtils
{
public:
  static vector<AudioObjectID> GetAllDeviceIDs();
  static vector<string> GetAllDeviceNames();
  static AudioObjectID GetDefaultDeviceID(bool aInput);
};

#endif // #ifndef AUDIODEVICEUTILS_H