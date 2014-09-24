#include "midiwrapper.h"
#include "RtMidi.h"

MidiWrapper::MidiWrapper(QObject *parent) :
    QObject(parent)
{
    midiIn = new RtMidiIn(RtMidi::MACOSX_CORE);
    midiOut = new RtMidiOut(RtMidi::MACOSX_CORE);

    midiIn->openVirtualPort("RecordingLightMidiIn");
    midiIn->setCallback(&staticCallback, this);
    midiIn->ignoreTypes(true, true, true);

    midiOut->openVirtualPort("RecordingLightMidiOut");
}

MidiWrapper::~MidiWrapper(){
    delete midiIn;
    delete midiOut;
}

void MidiWrapper::sendMessage(std::vector<unsigned char> msg){
    midiOut->sendMessage(&msg);
}

void MidiWrapper::staticCallback(double timeStamp, std::vector< unsigned char > *message, void *userData){
    MidiWrapper* myInstance = static_cast<MidiWrapper*>(userData);
    myInstance->instanceCallback(timeStamp, message);
}

void MidiWrapper::instanceCallback(double timeStamp, std::vector< unsigned char > *message){
    emit receivedMessage(timeStamp, *message);
}
