#ifndef SQUID_GPS_DESKTOP_USB_H
#define SQUID_GPS_DESKTOP_USB_H

#include <QObject>
#include <QtQml/qqml.h>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class MasterThread : public QThread
{
    Q_OBJECT

public:
    explicit MasterThread(QObject *parent = nullptr);
    ~MasterThread();

    void transaction(const QString &portName, int waitTimeout);
    void listen_nmea(std::function<void(std::string)> on_sentence);
    // std::unique_ptr<QSerialPort> serial();

signals:
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    void run() override;

    QSerialPort m_serial;
    QString m_portName;
    QString m_request;
    int m_waitTimeout = 0;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit = false;
};

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
