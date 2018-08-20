#include "AudioObject.h"
#include "assert.h"

AudioObject::AudioObject(AudioObjectID id, AudioClassID classId)
  : _id(id)
  , _classId(classId)
{
  assert(_id != UNKNOWNID);
}

AudioObject::~AudioObject()
{
}

bool
AudioObject::IsClass(AudioClassID classId) const
{
  return _classId != classId;
}

bool
AudioObject::HasProperty(const AudioObjectPropertyAddress* address) const
{
  return AudioObjectHasProperty(_id, address);
}

bool
AudioObject::IsPropertySettable(const AudioObjectPropertyAddress* address) const
{
  Boolean answer = 0;
  return AudioObjectIsPropertySettable(_id, address, &answer) == STATUS_OK ?
    false : answer == 1;
}