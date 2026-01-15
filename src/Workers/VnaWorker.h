// Worker module. Performs measurements in a separate thread.

#ifndef VNAWORKER_H
#define VNAWORKER_H

#include "Interfaces/IVnaModel.h"
#include "Model/VnaScpiClient.h"

#include <QObject>
#include <QMetaObject>
#include <QTimer>
#include <qDebug>
#include <QThread>


// !! Runs in a separate thread. !!
class VnaWorker : public QObject
{
    Q_OBJECT

public:
    explicit VnaWorker(QObject *parent = nullptr);
    virtual ~VnaWorker() = default;

public slots:
    // ScpiClient thread initialization.
    void initialize();

    // View->Presenter: Measure button signal.
    void startMeasurement();

    // View->Presenter: Measure button signal with parameters.
    void startMeasurementWithParams(double startFreq,
                                    double stopFreq,
                                    int points,
                                    double power,
                                    int ifBw);

    // Successful connection to socket.
    void onConnected();

    // Socket disconnect.
    void onDisconnected();

    // The connection was not established after a timeout.
    void onConnectionTimeout();

    // Execute a request for measurements.
    void performMeasurement();

    // Execute a request for measurements with parameters.
    void performMeasurementWithParams(double startFreq,
                                        double stopFreq,
                                        int points,
                                        double power,
                                        int ifBw);

    // Model->Worker->Presenter: Transport data from socket.
    void onConfigurationReceived(double startFreq,
                                    double stopFreq,
                                    int points,
                                    double power,
                                    int ifBw);

    // Model->Worker->Presenter: Transport graph data from socket.
    void onSParametersReceived(const QString& data);

signals:
    // Model->Worker->Presenter: Application status transport signal.
    void statusChanged(const QString& msg);

    // Model->Worker->Presenter:
    void sendCommand(const QString& msg);

    // Receive data from the socket.
    void configurationReceived(double startFreq,
                                double stopFreq,
                                int points,
                                double power,
                                int ifBw);

    // Receiving graph data from the socket.
    void sParametersReceived(const QString& rawData);

private slots:
    // Automatic chart update.
    void requestGraphOnly();

private:
    // Data request status: with parameters / no parameters.
    enum class MeasurementType {
        None,
        Simple,
        WithParams
    };
    // Temporary storage of data for calling a method with parameters.
    MeasurementType m_pendingMeasurement = MeasurementType::None;

    double m_pendingStart = 0.0;
    double m_pendingStop = 0.0;
    int m_pendingPoints = 0;
    double m_pendingPower = 0.0;
    int m_pendingIfBw = 0;

    // Connecting to the device.
    void attemptConnect();

    ScpiClient* m_client = nullptr;
    QTimer *m_timer = nullptr;
    QTimer* m_autoUpdateTimer = nullptr;
};


#endif // VNAWORKER_H
