#ifndef MIDICONNECTIONS_H
#define MIDICONNECTIONS_H

#include <QObject>
#include <vector>

class RtMidiIn;
class RtMidiOut;

class MidiWrapper : public QObject
{
    Q_OBJECT
public:
    explicit MidiWrapper(QObject *parent = 0);
    ~MidiWrapper();

signals:
    void receivedMessage(double timestamp, std::vector<unsigned char> msg);

public slots:
    void sendMessage(std::vector<unsigned char> msg);

private:
    RtMidiIn* midiIn;
    RtMidiOut* midiOut;

    static void staticCallback(double timeStamp, std::vector< unsigned char > *message, void *userData);
    void instanceCallback(double timeStamp, std::vector< unsigned char > *message);
};

#endif // MIDICONNECTIONS_H
