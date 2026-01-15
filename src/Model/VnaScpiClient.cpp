// IVnaModel implementation - inherits from abstract interfaces.
// Works in a thread with QTcpSocket, sending/receiving SCPI commands.

#include "Model/VnaScpiClient.h"


// !! Runs in a separate thread. !!
ScpiClient::ScpiClient(QObject *parent)
    : IModel(parent)
{
    // Create a socket.
    m_socket = new QTcpSocket(this);

    // Standard socket connections.
    connect(m_socket, &QTcpSocket::connected, this, &ScpiClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ScpiClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ScpiClient::onReadyRead);
}

// Destructor, closing the socket connection.
ScpiClient::~ScpiClient()
{
    if (m_socket && m_socket->isOpen()) {
        m_socket->abort();
    }
}

// Connecting to a socket via host and port.
void ScpiClient::connectTo(const QString& host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

// Checking the connection status to the device.
bool ScpiClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

// Forcefully reset the socket connection.
void ScpiClient::abort()
{
    m_socket->abort();
}

// Successful connection to socket.
void ScpiClient::onConnected()
{
    emit connected();
}

// Socket disconnect warning.
void ScpiClient::onDisconnected()
{
    emit disconnected();
}

// Method for getting current socket data.
void ScpiClient::requestConfiguration()
{
    m_socket->write(":SENS:FREQ:STAR?\n");
    m_socket->write(":SENS:FREQ:STOP?\n");
    m_socket->write(":SENS:SWE:POIN?\n");
    m_socket->write(":SOUR:POW?\n");
    m_socket->write(":SENS:BAND?\n");
    m_socket->flush();
}

// Method for requesting data from view fields to a socket.
void ScpiClient::setConfiguration(double startFreq,
                                    double stopFreq,
                                    int points,
                                    double power,
                                    int ifBw)
{
    // Converting data to the required format (a hack).
    QString startStr = QString("%1E9").arg(startFreq, 0, 'f', 2);
    QString stopStr  = QString("%1E9").arg(stopFreq, 0, 'f', 2);
    QString powerStr  = QString("%1E9").arg(power, 0, 'f', 2);

    // SCPI requests in socket.
    m_socket->write(QString(":SENS:FREQ:STAR %1\n").arg(startStr).toUtf8());
    m_socket->write(QString(":SENS:FREQ:STOP %1\n").arg(stopStr).toUtf8());
    m_socket->write(QString(":SENS:SWE:POIN %1\n").arg(points).toUtf8());
    m_socket->write(QString(":SOUR:POW %1\n").arg(powerStr).toUtf8());
    m_socket->write(QString(":SENS:BAND %1\n").arg(ifBw).toUtf8());
    m_socket->flush();
}

// Receiving data for the desired graph from a socket.
void ScpiClient::requestSParamsGraph()
{
    m_socket->write(":FORM:DATA ASC\n");
    m_socket->write(":INIT:IMM\n");
    m_socket->write(":CALC:DATA:SDAT?\n");
    m_socket->flush();
}

// If data has arrived in the socket, we receive it.
void ScpiClient::onReadyRead()
{
    QString rawData = m_socket->readAll().trimmed();
    QStringList lines = rawData.split('\n', Qt::SkipEmptyParts);

    // Extracting data.
    if (lines.size() >= 5) {
        emit configurationReceived(
            lines[0].toDouble(),
            lines[1].toDouble(),
            lines[2].toInt(),
            lines[3].toDouble(),
            lines[4].toDouble()
            );
    }
    else {
        // Graph data.
        emit sParametersReceived(rawData);
    }
}
