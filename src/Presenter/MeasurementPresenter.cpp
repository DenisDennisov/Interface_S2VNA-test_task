// Presenter module. It is the link between Model and View.

#include "Presenter/MeasurementPresenter.h"


MeasurementPresenter::MeasurementPresenter(IView *view, IConfigModel *config, QObject *parent)
    : QObject(parent), view(view), config(config)
{
    // Create a new Worker thread.
    m_thread = new QThread(this);
    m_worker = new VnaWorker();
    m_worker->moveToThread(m_thread);
    m_thread->start();

    // Thread->Worker: Create ScpiClient inside the Worker.
    connect(m_thread, &QThread::started, m_worker, &VnaWorker::initialize);

    // View->Presenter: Clicking the button in the UI - "Measure".
    connect(view, &IView::measureRequested, this, &MeasurementPresenter::onHandleMeasureRequested);

    // Worker->View: Update the program's status.
    connect(m_worker, &VnaWorker::statusChanged, view, &IView::onStatusUpdated);
    // Worker->Presenter: Receive data from the socket (startFreq, stopFreq, points, power, ifBw)
    connect(m_worker, &VnaWorker::configurationReceived, this, &MeasurementPresenter::onConfigReceived);
    // Worker->Presenter: Receive graph data from the socket.
    connect(m_worker, &VnaWorker::sParametersReceived, this, &MeasurementPresenter::onDataReceived);

    // Presenter->Worker: Sending a signal from the "Measure" button.
    connect(this, &MeasurementPresenter::startFirstMeasure, m_worker, &VnaWorker::startMeasurement);
    // Presenter->Worker: We send a signal with data from the "Measure" button with the received data from the View.
    connect(this, &MeasurementPresenter::startParamsMeasure, m_worker, &VnaWorker::startMeasurementWithParams);
    // Presenter->View: Passing data to be displayed in the interface.
    connect(this, &MeasurementPresenter::paramsUpdated, view, &IView::onParamsUpdated);
    // Presenter->View: Passing the chart to the interface for display.
    connect(this, &MeasurementPresenter::graphUpdated, view, &IView::onSetupGraph);
}

// Destructor, close the stream.
MeasurementPresenter::~MeasurementPresenter()
{
    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
        m_thread->deleteLater();
    }
    m_worker->deleteLater();
}

// "Measure" button handler.
void MeasurementPresenter::onHandleMeasureRequested(double startFreq,
                                                    double stopFreq,
                                                    int points,
                                                    double power,
                                                    int ifBw)
{
    // Get the current config and compare it with the values.
    // If the client provides new values, save them in Config and send the new values ​​to the socket.
    // If there are no changes, don't update Config.
    VnaConfig cfg = config->getConfig();
    if (cfg.startFreq != startFreq ||
        cfg.stopFreq != stopFreq  ||
        cfg.points != points ||
        cfg.power != power ||
        cfg.ifBw != ifBw)
    {
        setConfig(startFreq, stopFreq, points, power, ifBw);
        emit startParamsMeasure(startFreq, stopFreq, points, power, ifBw);
    } else
        emit startFirstMeasure();

}

// Setting parameters in the setter.
void MeasurementPresenter::setConfig(double startFreq,
                                     double stopFreq,
                                     int points,
                                     double power,
                                     int ifBw)
{
    VnaConfig newCfg = { startFreq, stopFreq, points, power, ifBw };
    config->setConfig(newCfg);
}

// Graph plotting method.
// I'm save in Presenter, but VnaConfig run in the main thread.
QVector<QPointF> MeasurementPresenter::parseSGraph(const QString& rawData)
{
    //Get current configuration.
    VnaConfig cfg = config->getConfig();

    QVector<QPointF> result;

    // Split raw comma-separated string into individual numeric tokens.
    // Example input: 0.1, -0.2 ,0.3...
    QStringList parts = rawData.split(',', Qt::SkipEmptyParts);

    // Use configured number of points; fall back to 101 if invalid.
    int numPoints = cfg.points > 0 ? cfg.points : 101;
    if (numPoints <= 1) numPoints = 2;

    // Convert start/stop frequencies from GHz (as stored) to Hz for internal calculations.
    double startHz = cfg.startFreq * 1e9;
    double stopHz = cfg.stopFreq * 1e9;

    // Parse real/imaginary pairs and convert to magnitude (dB) vs frequency (MHz).
    for (int i = 0; i < numPoints && i * 2 + 1 < parts.size(); ++i) {
        bool ok1, ok2;
        double re = parts[2 * i].toDouble(&ok1);            // Real part.
        double im = parts[2 * i + 1].toDouble(&ok2);        // Imaginary part.

        if (!ok1 || !ok2) continue;

        // Compute magnitude: |S| = sqrt(Re^2 + Im^2).
        double mag = std::sqrt(re * re + im * im);

        // Convert to decibels: 20 * log10(|S|).
        // Clamp to 1e-12 to avoid log(0) → -inf.
        double magDb = 20.0 * std::log10(std::max(mag, 1e-12));

        // Linearly interpolate frequency across the sweep range.
        double freqHz = startHz + (stopHz - startHz) * i / (numPoints - 1);

        // Store as (frequency in MHz, magnitude in dB) — standard units for RF plots.
        result.append(QPointF(freqHz / 1e6, magDb));
    }
    return result;
}

// Worker->View: transport parameters signal.
void MeasurementPresenter::onConfigReceived(double startFreq,
                                            double stopFreq,
                                            int points,
                                            double power,
                                            int ifBw)
{
    emit paramsUpdated(startFreq / 1e9, stopFreq / 1e9, points, power / 1e9, ifBw);
    // Save the received data to the config.
    setConfig(startFreq / 1e9, stopFreq / 1e9, points, power, ifBw);
}

// Worker->Presenter: schedule transport signal (build and transmit).
void MeasurementPresenter::onDataReceived(const QString& rawData)
{
    QVector<QPointF> data = parseSGraph(rawData);
    VnaConfig cfg = config->getConfig();
    // Graph data, startFreq and stopFreq to update the X axis.
    emit graphUpdated(data, cfg.startFreq, cfg.stopFreq);
}
