# ABC: AudioBehaviorCheck

This is used to test the behaviors of CoreAudio APIs.
It's originally imported from [my gist post][gist].

## How to use
Clone this repo and run ```$ make all```.
You can use ```$ make clean```

## TODO
- Implement a state callback to notify _started_, _stopped_, or _drained_
  - Maybe we should turn ```AudioStream``` into _FSM_ style
- Enable *input* for ```AudioStream```
- Try using AudioUnit with only *Output* scope
- Try using AudioUnit with only *Input* scope
- Try using AudioUnit with both *Input* and *Output* scopes
- Try using ```kAudioUnitSubType_VoiceProcessingIO``` in AudioUnit
  and see what the differences are from the above.
- Able to let DeviceChangeCallback to notify users what exact change is

[gist]: https://gist.github.com/ChunMinChang/47b8712ed57b96721eec18dede39d2f9 "Note for coreaudio"
