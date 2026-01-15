// Worker module. Performs measurements in a separate thread.

#include "VnaWorker.h"


// !! Runs in a separate thread. !!
VnaWorker::VnaWorker(QObject *parent)
    : QObject(parent)
{
    // Connect a timer to reconnect to the socket.
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &VnaWorker::onConnectionTimeout);
}

// ScpiClient thread initialization.
void VnaWorker::initialize()
{
    if (m_client) return;
    m_client = new ScpiClient(this);

    // Model->VnaWorker: Socket status: connected / disconnected.
    connect(m_client, &ScpiClient::connected, this, &VnaWorker::onConnected);
    connect(m_client, &ScpiClient::disconnected, this, &VnaWorker::onDisconnected);
    // Model->VnaWorker: Received data from the socket.
    connect(m_client, &ScpiClient::configurationReceived, this, &VnaWorker::onConfigurationReceived);
    // Model->VnaWorker: Received schedule from the socket.
    connect(m_client, &ScpiClient::sParametersReceived, this, &VnaWorker::onSParametersReceived);
}

// "Measure" button signal.
void VnaWorker::startMeasurement()
{
    if (m_client->isConnected()) {
        performMeasurement();
    } else {
        // Remember to make a "Simple" measurement after connecting.
        m_pendingMeasurement = MeasurementType::Simple;
        attemptConnect();
    }
}

// "Measure" button signal with parameters.
void VnaWorker::startMeasurementWithParams(double startFreq,
                                            double stopFreq,
                                            int points,
                                            double power,
                                            int ifBw)
{
    if (m_client->isConnected()) {
        performMeasurementWithParams(startFreq, stopFreq, points, power, ifBw);
    } else {
        // Remember to make a "WithParams" measurement after connecting and save params.
        m_pendingMeasurement = MeasurementType::WithParams;
        m_pendingStart = startFreq;
        m_pendingStop = stopFreq;
        m_pendingPoints = points;
        m_pendingPower = power;
        m_pendingIfBw = ifBw;
        attemptConnect();
    }
}

// Connecting to the socket with host and port.
void VnaWorker::attemptConnect()
{
    m_timer->stop();                // Stop old timer.
    m_client->abort();              // Reset the old connection.
    emit statusChanged("Status: Initiating connection...");
    m_client->connectTo("127.0.0.1", 5025);
    m_timer->start(5000);           // Trying 5 sec.
}

// Successful connection to socket.
void VnaWorker::onConnected()
{
    m_timer->stop();
    emit statusChanged("Status: Connected to S2VNA!");

    // Start timer automatic graph update.
    if (!m_autoUpdateTimer) {
        m_autoUpdateTimer = new QTimer(this);
        connect(m_autoUpdateTimer, &QTimer::timeout, this, &VnaWorker::requestGraphOnly);
    }
    m_autoUpdateTimer->start(300);

    // Call the required method after connection: without parameters / with parameters.
    switch (m_pendingMeasurement)
    {
        case MeasurementType::Simple:
            performMeasurement();
            break;

        case MeasurementType::WithParams:
            performMeasurementWithParams(
                m_pendingStart, m_pendingStop, m_pendingPoints,
                m_pendingPower, m_pendingIfBw
                );
            break;

        case MeasurementType::None:
            break;
    }
    // Reset state.
    m_pendingMeasurement = MeasurementType::None;
}

// Socket disconnect.
void VnaWorker::onDisconnected()
{
    emit statusChanged("Status: Connection lost...");
    m_pendingMeasurement = MeasurementType::None;

    // Stop timer automatic graph update.
    if (m_autoUpdateTimer) {
        m_autoUpdateTimer->stop();
    }
}

// The connection was not established after a timeout.
void VnaWorker::onConnectionTimeout()
{
    emit statusChanged("Status: Not connected to server...");
    m_pendingMeasurement = MeasurementType::None;
}

// Execute a request for measurements.
void VnaWorker::performMeasurement()
{
    m_client->requestConfiguration();
}

// Execute a request for measurements with parameters.
void VnaWorker::performMeasurementWithParams(double startFreq,
                                                double stopFreq,
                                                int points,
                                                double power,
                                                int ifBw)
{
    m_client->setConfiguration(startFreq, stopFreq, points, power, ifBw);

}

// Model->Worker->Presenter: Transport data from socket.
void VnaWorker::onConfigurationReceived(double startFreq,
                                            double stopFreq,
                                            int points,
                                            double power,
                                            int ifBw)
{
    emit configurationReceived(startFreq, stopFreq, points, power, ifBw);
}

// Model->Worker->Presenter: Transport graph data from socket.
void VnaWorker::onSParametersReceived(const QString& rawData)
{
    emit sParametersReceived(rawData);
}

// Automatic chart update.
void VnaWorker::requestGraphOnly()
{
    if (!m_client || !m_client->isConnected()) {
        if (m_autoUpdateTimer) {
            m_autoUpdateTimer->stop();
        }
        return;
    }
    m_client->requestSParamsGraph();
}
