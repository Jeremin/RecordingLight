#include "midihandler.h"

#include <QDebug>
#include <QMutexLocker>

#define MIDI_CC_TYPE    0xb0
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80

const uint8_t playbackControlZone = 0x0e;
const uint8_t stopPort = 0x03;
const uint8_t playPort = 0x04;
const uint8_t recPort = 0x05;

MidiHandler::MidiHandler(QObject *parent) :
    QThread(parent)
{
    m_status = Idle;
    m_isStopRequested = true;
    m_currentHuiZone = 0;

    qRegisterMetaType<std::vector<unsigned char> >("std::vector<unsigned char>");
}

MidiHandler::MidiHandlerStatus MidiHandler::getStatus(){
    return m_status;
}

void MidiHandler::stop(){
    m_isStopRequested = true;
}

void MidiHandler::receiveMessage(double timestamp, std::vector<unsigned char> msg){
    Q_UNUSED(timestamp);
    m_messageBufferMutex.lock();
    m_messageBuffer.append(msg);
    m_messageBufferMutex.unlock();
}

void MidiHandler::run(){
    m_isStopRequested = false;
    while(!m_isStopRequested){
        if (!handleMessages()){
            msleep(10);
        }
    }
}

bool MidiHandler::handleMessages(){
    QMutexLocker locker(&m_messageBufferMutex);
    if (m_messageBuffer.size()>0){
        foreach (std::vector<unsigned char> msg, m_messageBuffer){
            handleOneMessage(&msg);
        }
        m_messageBuffer.clear();
        return true;
    }
    else
    {
        return false;
    }
}

void MidiHandler::handleOneMessage(std::vector<unsigned char> *msg){
    if (msg->size()<3){
        return;
    }
    uint8_t msgType = msg->at(0);
    switch (msgType){
        case MIDI_NOTE_OFF:
        handleNoteOffMessage(msg);
        break;
    case MIDI_CC_TYPE:
        handleControlChangeMessage(msg);
        break;
    default:
        break;
    }
    return;
}

void MidiHandler::handleNoteOffMessage(std::vector<unsigned char> *msg){
    if (isPing(msg)){
        std::vector<unsigned char> response;
        response.push_back(0x90);
        response.push_back(0x00);
        response.push_back(0x7f);
        emit sendMessage(response);
    }
}

void MidiHandler::handleControlChangeMessage(std::vector<unsigned char> *msg){
    bool isPortOn;
    uint8_t data;

    if (isZoneSelect(msg, &data)){
        m_currentHuiZone = data;
        return;
    }
    else if (isPortOnOff(msg, &isPortOn, &data)){
        handlePortChange(isPortOn, data);
        return;
    }
}

void MidiHandler::handlePortChange(bool isOn, uint8_t port){
    if (m_currentHuiZone == playbackControlZone){
        MidiHandlerStatus statusAtStart = m_status;
        switch (m_status){
        case Idle:
            if (isOn && port==recPort){
                m_status = Ready;
            }
            else if (isOn && port==playPort){
                m_status = Cue;
            }
            break;
        case Ready:
            if (isOn && port==recPort){
                m_status = Idle;
            }
            else if (isOn && port==playPort){
                m_status = Recording;
            }
            break;
        case Recording:
            if (isOn && port==stopPort){
                m_status = Idle;
            }
            break;
        case Cue:
            if (isOn && port==stopPort){
                m_status = Idle;
            }
            break;
        }

        if (m_status != statusAtStart){
            emit statusChanged();
        }
    }
}

bool MidiHandler::isPing(std::vector<unsigned char> *msg){
    return (msg->at(1)==0 && msg->at(2)==0);
}

bool MidiHandler::isZoneSelect(std::vector<unsigned char> *msg, uint8_t* zone){
    if (msg->at(1)==0x0c){
        *zone = msg->at(2);
        return true;
    }
    return false;
}

bool MidiHandler::isPortOnOff(std::vector<unsigned char> *msg, bool *isOn, uint8_t* port){
    if (msg->at(1)==0x2c){
        uint8_t data = msg->at(2);
        *isOn = (0xf0&data)==0x40;
        *port = (0x0f&data);
        return true;
    }
    return false;
}
