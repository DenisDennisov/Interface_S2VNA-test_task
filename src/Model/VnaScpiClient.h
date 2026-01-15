// IVnaModel implementation from abstract interfaces.
// Works in a thread with QTcpSocket, sending/receiving SCPI commands.

#ifndef VNASCPICLIENT_H
#define VNASCPICLIENT_H

#include "Interfaces/IVnaModel.h"

#include <QTimer>
#include <QTcpSocket>
#include <QThread>


// !! Runs in a separate thread. !!
class ScpiClient : public IModel
{
    Q_OBJECT

public:
    explicit ScpiClient(QObject *parent = nullptr);
    ~ScpiClient() override;

    void connectTo(const QString& host, quint16 port) override;

    bool isConnected() const override;

    void abort() override;

    void requestConfiguration() override;

    void setConfiguration(double startFreq,
                            double stopFreq,
                            int points,
                            double power,
                            int ifBw) override;

    void requestSParamsGraph() override;

private slots:
    // Successful connection to socket.
    void onConnected();

    // Socket disconnect warning.
    void onDisconnected();

    // Get contents from socket (data/graph).
    void onReadyRead();

private:
    QTcpSocket *m_socket = nullptr;

};

#endif // VNASCPICLIENT_H
