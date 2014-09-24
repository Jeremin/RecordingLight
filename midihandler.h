#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <QThread>
#include <QMutex>
#include <QVector>
#include <vector>

class MidiHandler : public QThread
{
    Q_OBJECT
public:
    enum MidiHandlerStatus{Idle, Ready, Recording, Cue};

    explicit MidiHandler(QObject *parent = 0);

    MidiHandlerStatus getStatus();
    void stop();

signals:
    void sendMessage(std::vector<unsigned char> msg);
    void statusChanged();

public slots:
    void receiveMessage(double timestamp, std::vector<unsigned char> msg);

protected:
    void run();

private:
    bool handleMessages();
    void handleOneMessage(std::vector<unsigned char> *msg);

    void handleNoteOffMessage(std::vector<unsigned char> *msg);
    void handleControlChangeMessage(std::vector<unsigned char> *msg);

    void handlePortChange(bool isOn, uint8_t port);

    //noteoff types
    bool isPing(std::vector<unsigned char> *msg);

    //cc types
    bool isZoneSelect(std::vector<unsigned char> *msg, uint8_t* zone);
    bool isPortOnOff(std::vector<unsigned char> *msg, bool *isOn, uint8_t* port);

    QVector<std::vector<unsigned char> > m_messageBuffer;
    QMutex m_messageBufferMutex;

    bool m_isStopRequested;

    MidiHandlerStatus m_status;
    uint8_t m_currentHuiZone;
};

#endif // MIDIHANDLER_H
