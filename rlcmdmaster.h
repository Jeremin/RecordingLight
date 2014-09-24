#ifndef RLCMDMASTER_H
#define RLCMDMASTER_H

#include "SerialProtocol.h"
#include "bluetoothserialport.h"

#include <QObject>
#include <QVector>

class QTimer;

class RLCmdMaster : public QObject
{
    Q_OBJECT
public:
    explicit RLCmdMaster(QObject *parent = 0);
    virtual ~RLCmdMaster();

    enum RLCmdMaster_Status {NOTCONNECTED, OK, ERROR};
    RLCmdMaster_Status getStatus();

    void open();
    void close();

signals:
    void statusChanged(RLCmdMaster_Status status);
    void receivedResponse(RLMsg msg);

public slots:
    void sendCmd(RLMsg msg);

private slots:
    void transmissionTimeout();
    void timerTimeout();

private:
    void setNewStatus(RLCmdMaster_Status newStatus);
    void readPort();
    void handleData();

    bool isHeader();
    void getResponse();

    RLCmdMaster_Status m_status;
    BluetoothSerialPort m_serialPort;
    QTimer* m_timer;
    QTimer* m_timeout;
    bool m_isTransmissionInProgress;
    QList<uint8_t> m_receivedData;
};

#endif // RLCMDMASTER_H
