// Presenter module. It serves as the link between the Model (via the Worker) and the View:
// Receives/updates View events, manages the Worker, and parses data.

#ifndef MEASUREMENTPRESENTER_H
#define MEASUREMENTPRESENTER_H

#include "Interfaces/IVnaView.h"
#include "Interfaces/IVnaConfig.h"
#include "Workers/VnaWorker.h"

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QThread>
#include <cmath>


class MeasurementPresenter : public QObject
{
    Q_OBJECT

public:
    // Implementation DI: Interface and Config (Model).
    explicit MeasurementPresenter(IView *view,
                                  IConfigModel *config,
                                  QObject *parent = nullptr);
    virtual ~MeasurementPresenter();

signals:
    // Signal from the "Measure" button.
    void startFirstMeasure();

    // Signal with data from the "Measure" button with the data received from View.
    void startParamsMeasure(double startFreq,
                            double stopFreq,
                            int points,
                            double power,
                            int ifBw);

    // Presenter->View: Updating data in the interface.
    void paramsUpdated(double startFreq,
                        double stopFreq,
                        int points,
                        double power,
                        int ifBw);

    // Presenter->View: Update graph interface.
    void graphUpdated(const QVector<QPointF> data,
                        double startFreq,
                        double stopFreq);

private slots:
    // "Measure" button handler.
    void onHandleMeasureRequested(double startFreq,
                                    double stopFreq,
                                    int points,
                                    double power,
                                    int ifBw);

    // Setting parameters in the setter in Config.
    void setConfig(double startFreq,
                    double stopFreq,
                    int points,
                    double power,
                    int ifBw);

    // Worker->View: transport parameters signal.
    void onConfigReceived(double startFreq,
                            double stopFreq,
                            int points,
                            double power,
                            int ifBw);

    // Worker->Presenter: schedule transport signal (build and transmit).
    void onDataReceived(const QString& rawData);

private:
    // Method for plotting a graph.
    QVector<QPointF> parseSGraph(const QString& rawData);

    IView *view;
    IConfigModel *config;
    VnaWorker *m_worker = nullptr;
    QThread *m_thread;
};

#endif // MEASUREMENTPRESENTER_H
