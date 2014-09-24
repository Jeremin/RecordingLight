#include "bluetoothserialport.h"

#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <QDebug>
#include <QString>

static QString serialPortPath = "/dev/cu.WIDE_HK-DevB";

BluetoothSerialPort::BluetoothSerialPort(QObject *parent) :
    QObject(parent)
{
    m_fileDescriptor = -1;
    m_isOpen = false;
}

BluetoothSerialPort::~BluetoothSerialPort(){
    close();
}

void BluetoothSerialPort::open(){
    if (!m_isOpen){
        m_fileDescriptor = ::open(serialPortPath.toAscii().data(), O_RDWR | O_NOCTTY);

        if (-1 == m_fileDescriptor){
            qDebug() << "Error opening serial port";
            return;
        }

        if (!m_setSerialConfig()){
            qDebug() << "Error setting serial settings";
            return;
        }

        m_isOpen = true;
    }
}

void BluetoothSerialPort::close(){
    if (m_isOpen && m_fileDescriptor!=-1){
        if (0 != ::close(m_fileDescriptor)){
            qDebug() << "Error closing serial port";
        }
    }

    m_isOpen = false;
    m_fileDescriptor = -1;
}

bool BluetoothSerialPort::isOpen(){
    return m_isOpen;
}

int BluetoothSerialPort::write(void* data, uint32_t size){
    if (m_isOpen){
        return ::write(m_fileDescriptor, data, size);
    }
    return -1;
}

int BluetoothSerialPort::read(void* data, uint32_t size){
    if (m_isOpen){
        return ::read(m_fileDescriptor, data, size);
    }
    return -1;
}

bool BluetoothSerialPort::m_setSerialConfig(){
    struct termios options;
    if (-1 == ioctl(m_fileDescriptor, TIOCEXCL))
    {
        return false;
    }

    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_cflag = 0;
    cfmakeraw(&options);
    cfsetspeed(&options, B38400);
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 1;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag |= (CS8 | CREAD | CLOCAL);

    if (-1 == tcsetattr(m_fileDescriptor, TCSANOW, &options)) {
        return false;
    }

    return true;
}
