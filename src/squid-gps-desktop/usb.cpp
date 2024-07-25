#include "usb.h"

#include <QSerialPort>
#include <QTime>

MasterThread::MasterThread(QObject *parent) :
    QThread(parent)
{

}


MasterThread::~MasterThread()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

void MasterThread::transaction(const QString &portName, int waitTimeout)
{
    const QMutexLocker locker(&m_mutex);
    m_portName = portName;
    m_waitTimeout = waitTimeout;
    m_request = "";

    if (!isRunning()) {
        start();
    }
    else {
        m_cond.wakeOne();
    }
}

void MasterThread::run()
{
    bool currentPortNameChanged = false;

    m_mutex.lock();

    QString currentPortName;
    if (currentPortName != m_portName) {
        currentPortName = m_portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = m_waitTimeout;
    QString currentRequest = m_request;
    m_mutex.unlock();

    if (currentPortName.isEmpty()) {
        emit error(tr("No port name specified"));
        return;
    }

    while (!m_quit) {
        if (currentPortNameChanged) {
            m_serial.close();
            m_serial.setPortName(currentPortName);
            m_serial.setBaudRate(4800);

            if (!m_serial.open(QIODevice::ReadOnly)) {
                emit error(tr("Can't open %1, error code %2")
                               .arg(m_portName).arg(m_serial.error()));
                return;
            }
        }
        qDebug() << "\n Transaction INSIDE is reponse1...";

        // to improve!!
        while (true) {
            QByteArray responseData = m_serial.readAll();
            while (m_serial.waitForReadyRead(10)) {
                responseData += m_serial.readAll();
            }
            QString serialString = responseData.mid(responseData.indexOf("$"),responseData.indexOf("\r\n") + 2);
            if (!serialString.isEmpty())
                qDebug() << "SerialString" <<serialString;
        }

        m_mutex.lock();
        m_cond.wait(&m_mutex);
        if (currentPortName != m_portName) {
            currentPortName = m_portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = m_waitTimeout;
        currentRequest = m_request;
        m_mutex.unlock();
    }
}
