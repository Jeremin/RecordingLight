#ifndef RECORDINGLIGHTSTATEMACHINE_H
#define RECORDINGLIGHTSTATEMACHINE_H

#include "midihandler.h"
#include "rlcmdmaster.h"

#include <QObject>
#include <QColor>
#include <QList>

class MidiWrapper;
class RLCmdMaster;

class RecordingLightStateMachine : public QObject
{
    Q_OBJECT
public:
    explicit RecordingLightStateMachine(QObject *parent = 0);
    virtual ~RecordingLightStateMachine();

    void connectToDevice();
    void disconnectFromDevice();

    MidiHandler::MidiHandlerStatus getRecordingStatus();
    RLCmdMaster::RLCmdMaster_Status getConnectionStatus();

    enum colorHolder{idle, ready, rec, cue};

signals:
    void recordingStatusChanged();
    void connectionStatusChanged();

    void receivedColor(colorHolder holder, QColor col);

public slots:
    void sendTestMessage();

    void requestColor(colorHolder holder);
    void setColor(colorHolder holder, QColor col);

private:
    MidiWrapper* m_midiConnections;
    MidiHandler* m_midiHandler;
    RLCmdMaster* m_RLCmdMaster;

    QList<RLMsg> m_messageQueue;
    bool m_isTransmissionInProgress;

    void enqueueMessage(RLMsg msg);
    void handleResponse(RLMsg resp);
    void handleGetColorResponse(RLMsg* msg);

private slots:
    void receivedResponse(RLMsg msg);

    void m_recordingStatusChanged();
};

#endif // RECORDINGLIGHTSTATEMACHINE_H
