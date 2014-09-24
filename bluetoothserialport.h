#ifndef BLUETOOTHSERIALPORT_H
#define BLUETOOTHSERIALPORT_H

#include <QObject>

class BluetoothSerialPort : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothSerialPort(QObject *parent = 0);
    virtual ~BluetoothSerialPort();

    void open();
    void close();

    bool isOpen();

signals:

public slots:
    int write(void* data, uint32_t size);
    int read(void* data, uint32_t size);

private:
    bool m_isOpen;
    int m_fileDescriptor;

    bool m_setSerialConfig();
};

#endif // BLUETOOTHSERIALPORT_H
