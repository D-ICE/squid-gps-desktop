#include "usb.h"
#include <QSerialPortInfo>
#include <QSerialPort>

USB::USB(QObject *parent) : QObject(parent),
    m_serial_port{new QSerialPort()}
{
    refresh();
}

QStringList USB::availablePorts() {
    return m_availablePorts;
}

void USB::refresh() {
    m_availablePorts.clear();
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPortInfos) {
        m_availablePorts.append(portInfo.portName());
        qDebug() << "\n"
                 << "Port:" << portInfo.portName() << "\n"
                 << "Location:" << portInfo.systemLocation() << "\n"
                 << "Description:" << portInfo.description() << "\n"
                 << "Manufacturer:" << portInfo.manufacturer() << "\n"
                 << "Serial number:" << portInfo.serialNumber() << "\n"
                 << "Vendor Identifier:"
                 << (portInfo.hasVendorIdentifier()
                         ? QByteArray::number(portInfo.vendorIdentifier(), 16)
                         : QByteArray()) << "\n"
                 << "Product Identifier:"
                 << (portInfo.hasProductIdentifier()
                         ? QByteArray::number(portInfo.productIdentifier(), 16)
                         : QByteArray());
    }

    emit availablePortsChanged();
}
