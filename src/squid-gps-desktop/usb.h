#ifndef SQUID_GPS_DESKTOP_USB_H
#define SQUID_GPS_DESKTOP_USB_H

#include <QObject>
#include <QtQml/qqml.h>

#include <QSerialPort>
#include <QSerialPortInfo>

class USB: public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY availablePortsChanged)

public:
    USB(QObject *parent = nullptr);

    QStringList availablePorts();
    Q_INVOKABLE void refresh();

signals:
    void availablePortsChanged();

private:
    std::unique_ptr<QSerialPort> m_serial_port;
    QStringList m_availablePorts;
};

#endif // SQUID_GPS_DESKTOP_USB_H
