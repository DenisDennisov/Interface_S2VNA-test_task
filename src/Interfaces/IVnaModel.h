// Abstract interface Model.
// Allows you to replace the implementation without changing the logic.

#ifndef IVNAMODEL_H
#define IVNAMODEL_H

#include <QString>
#include <QObject>


// We inherit from QObject in an abstract class,
// to communicate with the Worker via signals from the Presenter.
class IModel : public QObject
{
    Q_OBJECT

public:
    explicit IModel(QObject *parent = nullptr)
        : QObject(parent){}
    virtual ~IModel() = default;

    // Connect to the socket.
    virtual void connectTo(const QString& host, quint16 port) = 0;

    // Check if the socket is connected.
    virtual bool isConnected() const = 0;

    // Forcefully reset the socket connection.
    virtual void abort() = 0;

    // Method for getting current socket data.
    virtual void requestConfiguration() = 0;

    // Method for requesting your data to a socket.
    virtual void setConfiguration(double startFreq,
                                    double stopFreq,
                                    int points,
                                    double power,
                                    int ifBw) = 0;

    // Method for requesting the data graph from a socket from the S2VNA with SCPI.
    virtual void requestSParamsGraph() = 0;

signals:
    // Successful connection to socket.
    void connected();

    // Socket disconnect warning.
    void disconnected();

    // Receiving data from the socket.
    void configurationReceived(double startFreq,
                                double stopFreq,
                                int points,
                                double power,
                                int ifBw);

    // We receive graph data from the socket and update the X-axis.
    void sParametersReceived(const QString& rawData);

};

#endif // IVNAMODEL_H
