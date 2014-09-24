#include "rlcmdmaster.h"

#include <QDebug>
#include <QTimer>
#include <cstring>

const uint8_t packetSize = sizeof(RLMsg) + 1;

RLCmdMaster::RLCmdMaster(QObject *parent) :
    QObject(parent)
{
    m_status = NOTCONNECTED;
    m_isTransmissionInProgress = false;

    m_timer = new QTimer(this);
    m_timeout = new QTimer(this);

    m_timer->setInterval(10);
    m_timeout->setSingleShot(true);
    m_timeout->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    connect(m_timeout, SIGNAL(timeout()), this, SLOT(transmissionTimeout()));
}

RLCmdMaster::~RLCmdMaster(){
    delete m_timer;
    delete m_timeout;
}

RLCmdMaster::RLCmdMaster_Status RLCmdMaster::getStatus(){
    return m_status;
}

void RLCmdMaster::open(){
    m_serialPort.open();

    if (m_serialPort.isOpen()){
        setNewStatus(OK);
    }
}

void RLCmdMaster::close(){
    m_serialPort.close();
    setNewStatus(NOTCONNECTED);
}

void RLCmdMaster::sendCmd(RLMsg msg){
    if (OK == m_status){
        uint8_t dataBuffer[packetSize];
        m_timer->start();
        m_timeout->start();
        dataBuffer[0] = RL_CMD_HEADER;
        memcpy(dataBuffer+1, &msg, sizeof(RLMsg));
        int res = m_serialPort.write(dataBuffer, packetSize);
        if (res != packetSize){
            qDebug() << "only wrote" << res << "bytes";
            setNewStatus(ERROR);
        }
    }
}

void RLCmdMaster::transmissionTimeout(){
    m_isTransmissionInProgress = false;
    m_timer->stop();
    m_timeout->stop();
    qDebug() << "transmission timeout";
    setNewStatus(ERROR);
}

void RLCmdMaster::timerTimeout(){
    readPort();
    handleData();
}

void RLCmdMaster::setNewStatus(RLCmdMaster::RLCmdMaster_Status newStatus){
    if (m_status!=newStatus){
        m_status = newStatus;
        emit statusChanged(m_status);
    }
}

void RLCmdMaster::readPort(){
    uint8_t buffer[packetSize];
    int res = m_serialPort.read(buffer, packetSize);
    if (res>=0){
        for (int i = 0; i < res; ++i) {
            m_receivedData.push_back(buffer[i]);
        }
    }
    else {
        setNewStatus(ERROR);
    }
}

void RLCmdMaster::handleData(){
    if (m_receivedData.size()>=packetSize){
        while (m_receivedData.size()>=packetSize) {
            if (isHeader())
                getResponse();
        }
    }
}

bool RLCmdMaster::isHeader(){
    uint8_t data = m_receivedData.takeFirst();
    return RL_RESPONSE_HEADER == data;
}

void RLCmdMaster::getResponse(){
    RLMsg response;

    m_isTransmissionInProgress = false;
    m_timer->stop();
    m_timeout->stop();

    for (int i=0; i<(int)sizeof(RLMsg); i++){
        uint8_t data = m_receivedData.takeFirst();
        ((uint8_t*)&response)[i] = data;
    }

    emit receivedResponse(response);
}
