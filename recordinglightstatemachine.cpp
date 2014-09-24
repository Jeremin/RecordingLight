#include "recordinglightstatemachine.h"

#include "midiwrapper.h"

#include <QDebug>

RecordingLightStateMachine::RecordingLightStateMachine(QObject *parent) :
    QObject(parent)
{
    m_isTransmissionInProgress = false;

    m_midiConnections = new MidiWrapper();
    m_midiHandler = new MidiHandler();
    m_RLCmdMaster = new RLCmdMaster();

    connect(m_midiConnections, SIGNAL(receivedMessage(double,std::vector<unsigned char>)), m_midiHandler, SLOT(receiveMessage(double,std::vector<unsigned char>)));
    connect(m_midiHandler, SIGNAL(sendMessage(std::vector<unsigned char>)), m_midiConnections, SLOT(sendMessage(std::vector<unsigned char>)));
    connect(m_midiHandler, SIGNAL(statusChanged()), this, SLOT(m_recordingStatusChanged()));
    connect(m_RLCmdMaster, SIGNAL(statusChanged(RLCmdMaster_Status)), this, SIGNAL(connectionStatusChanged()));
    connect(m_RLCmdMaster, SIGNAL(receivedResponse(RLMsg)), this, SLOT(receivedResponse(RLMsg)));

    m_midiHandler->start();
}

RecordingLightStateMachine::~RecordingLightStateMachine(){
    disconnect(m_midiConnections);
    disconnect(m_midiHandler);

    m_midiHandler->stop();
    m_midiHandler->wait();

    delete m_RLCmdMaster;
    delete m_midiConnections;
    delete m_midiHandler;
}

void RecordingLightStateMachine::connectToDevice(){
    m_RLCmdMaster->open();
}

void RecordingLightStateMachine::disconnectFromDevice(){
    m_RLCmdMaster->close();
}

void RecordingLightStateMachine::sendTestMessage(){
    RLMsg msg;
    msg.cmd = RL_CMD_GET_STATE;
    msg.arg = 0;
    enqueueMessage(msg);
}

void RecordingLightStateMachine::requestColor(colorHolder holder){
    RLMsg msg;
    msg.arg = 0;
    switch (holder){
    case idle:
        msg.cmd = RL_CMD_GET_IDLE_COL;
        break;
    case ready:
        msg.cmd = RL_CMD_GET_READY_COL;
        break;
    case rec:
        msg.cmd = RL_CMD_GET_REC_COL;
        break;
    case cue:
        msg.cmd = RL_CMD_GET_CUE_COL;
        break;
    }
    enqueueMessage(msg);
}

void RecordingLightStateMachine::setColor(colorHolder holder, QColor col){
    RLMsg msg;
    msg.arg = 0;
    msg.arg |= 0xff000000 & col.red()<<24;
    msg.arg |= 0x00ff0000 & col.green()<<16;
    msg.arg |= 0x0000ff00 & col.blue()<<8;
    msg.arg |= 0x000000ff & col.alpha();

    switch (holder) {
    case idle:
        msg.cmd = RL_CMD_SET_IDLE_COL;
        break;
    case ready:
        msg.cmd = RL_CMD_SET_READY_COL;
        break;
    case rec:
        msg.cmd = RL_CMD_SET_REC_COL;
        break;
    case cue:
        msg.cmd = RL_CMD_SET_CUE_COL;
        break;
    }
    enqueueMessage(msg);
}

MidiHandler::MidiHandlerStatus RecordingLightStateMachine::getRecordingStatus(){
    return m_midiHandler->getStatus();
}

RLCmdMaster::RLCmdMaster_Status RecordingLightStateMachine::getConnectionStatus(){
    return m_RLCmdMaster->getStatus();
}

void RecordingLightStateMachine::enqueueMessage(RLMsg msg){
    if (m_RLCmdMaster->getStatus()==RLCmdMaster::OK){
        m_messageQueue.append(msg);
        if (!m_isTransmissionInProgress){
            m_isTransmissionInProgress = true;
            m_RLCmdMaster->sendCmd(msg);
        }
    }
}

void RecordingLightStateMachine::handleResponse(RLMsg resp){
    if (RL_CMD_GET_IDLE_COL<=resp.cmd && RL_CMD_GET_CUE_COL<=resp.cmd){
        handleGetColorResponse(&resp);
    }
}

void RecordingLightStateMachine::handleGetColorResponse(RLMsg* msg){
    uint8_t r,g,b,a;
    a = msg->arg & 0x000000ff;
    msg->arg >>= 8;
    b = msg->arg & 0x000000ff;
    msg->arg >>= 8;
    g = msg->arg & 0x000000ff;
    msg->arg >>= 8;
    r = msg->arg & 0x000000ff;

    QColor col(r,g,b,a);

    switch (msg->cmd) {
    case RL_CMD_GET_IDLE_COL:
        emit receivedColor(idle, col);
        break;
    case RL_CMD_GET_READY_COL:
        emit receivedColor(ready, col);
        break;
    case RL_CMD_GET_REC_COL:
        emit receivedColor(rec, col);
        break;
    case RL_CMD_GET_CUE_COL:
        emit receivedColor(cue, col);
        break;
    default:
        qDebug() << "Error handleGetColorResponse";
        break;
    }
}

void RecordingLightStateMachine::receivedResponse(RLMsg msg){
    qDebug() << "received something";
    if (m_messageQueue[0].cmd == msg.cmd){
        handleResponse(msg);
        m_messageQueue.removeFirst();
        if (m_messageQueue.size()){
            m_RLCmdMaster->sendCmd(m_messageQueue[0]);
        }
        else {
            m_isTransmissionInProgress = false;
        }
    }
    else {
        qDebug() << "received message, but refering to invalid CMD:" \
                    << hex << msg.cmd << "instead of" << hex << m_messageQueue[0].cmd;
        m_messageQueue.removeFirst();
    }
}

void RecordingLightStateMachine::m_recordingStatusChanged(){
    emit recordingStatusChanged();

    RLMsg msg;
    msg.arg = 0;

    switch(m_midiHandler->getStatus()){
    case MidiHandler::Idle:
        msg.cmd = RL_CMD_SET_IDLE;
        m_RLCmdMaster->sendCmd(msg);
        break;
    case MidiHandler::Ready:
        msg.cmd = RL_CMD_SET_READY;
        m_RLCmdMaster->sendCmd(msg);
        break;
    case MidiHandler::Recording:
        msg.cmd = RL_CMD_SET_REC;
        m_RLCmdMaster->sendCmd(msg);
        break;
    case MidiHandler::Cue:
        msg.cmd = RL_CMD_SET_CUE;
        m_RLCmdMaster->sendCmd(msg);
        break;
    default:
        qDebug() << "unknown status";
    }
}
