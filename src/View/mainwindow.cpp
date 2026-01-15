// View module. Displays the client UI and intercepts user actions.

#include "View/mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : IView(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MyTeskTask");

    // Styling the QChart chart.
    setupChart();

    // "Measure" button handle.
    connect(ui->measure_pushButton, &QPushButton::clicked, this, &MainWindow::onMeasureButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Click on the "Measure" button.
void MainWindow::onMeasureButtonClicked()
{
    // Getting parameters from UI fields.
    double startFreq = ui->startFreq_doubleSpinBox->value();     // Start, GHz
    double stopFreq = ui->stopFreq_doubleSpinBox->value();       // Stop, GHz
    int points = ui->points_spinBox->value();                    // points, int
    double power = ui->power_doubleSpinBox->value();             // power, dBm
    int ifBw = ui->ifBw_spinBox->value();                        // ifBw, Hz

    emit measureRequested(startFreq, stopFreq, points, power, ifBw);
}

// Changing the connection status.
void MainWindow::onStatusUpdated(const QString& msg)
{
    ui->status_label->setText(msg);
}

// Updating data in the interface.
void MainWindow::onParamsUpdated(double startFreq,
                                 double stopFreq,
                                 int points,
                                 double power,
                                 int ifBw)
{
    ui->startFreq_doubleSpinBox->setValue(startFreq);
    ui->stopFreq_doubleSpinBox->setValue(stopFreq);
    ui->points_spinBox->setValue(points);
    ui->power_doubleSpinBox->setValue(power);
    ui->ifBw_spinBox->setValue(ifBw);
}

// Draw the graph in the interface,
// changing the X-axis from the initial and final frequency.
void MainWindow::onSetupGraph(const QVector<QPointF>& data,
                              double startFreq,
                              double stopFreq)
{
    if (m_series) m_series->replace(data);
    updateXAxisRange(startFreq, stopFreq);
}

// Styling the graph chart.
void MainWindow::setupChart()
{
    // Series.
    m_series = new QLineSeries(this);
    m_series->setColor(Qt::yellow);
    m_series->setPen(QPen(Qt::yellow, 2));

    // Chart.
    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setBackgroundBrush(QBrush(Qt::black));
    m_chart->setPlotAreaBackgroundBrush(QBrush(Qt::black));
    m_chart->setPlotAreaBackgroundVisible(true);
    m_chart->setMargins(QMargins(0, 0, 0, 0));
    m_chart->setTitle("График");
    m_chart->setTitleBrush(QBrush(Qt::white));
    m_chart->legend()->setVisible(false);

    // Axis.
    m_axisX = new QValueAxis(this);
    m_axisY = new QValueAxis(this);

    // X-axis: default range (updated from data).
    m_axisX->setRange(0.1, 9000);           // 0.1 mHz – 9 GHz.
    m_axisX->setLabelsColor(Qt::white);
    m_axisX->setTitleBrush(QBrush(Qt::white));
    m_axisX->setGridLineColor(Qt::gray);
    m_axisX->setLabelFormat("%.0f");

    // Y axis: fixed.
    m_axisY->setRange(-50, 50);
    m_axisY->setLabelsColor(Qt::white);
    m_axisY->setTitleBrush(QBrush(Qt::white));
    m_axisY->setGridLineColor(Qt::gray);

    // Binding.
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    // View.
    ui->s11_chartView->setChart(m_chart);
    ui->s11_chartView->setBackgroundBrush(QBrush(Qt::black));
    ui->s11_chartView->setRenderHint(QPainter::Antialiasing);
}

// Dynamically update the X axis.
void MainWindow::updateXAxisRange(double startFreq, double stopFreq)
{
    if (!m_axisX) return;

    // Convert GHz to MHz.
    double startMhz = startFreq * 1000.0;
    double stopMhz = stopFreq * 1000.0;

    // Protection against incorrect values.
    if (startMhz <= 0) startMhz = 0.1;
    if (stopMhz <= startMhz) stopMhz = startMhz + 1.0;

    m_axisX->setRange(startMhz, stopMhz);
}
