// View module. Displays the client UI and intercepts user actions.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Interfaces/IVnaView.h"
#include "ui_mainwindow.h"

#include <QtCharts/QValueAxis>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public IView
{
    Q_INTERFACES(IView)

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void onStatusUpdated(const QString& msg) override;

    void onParamsUpdated(double startFreq,
                            double stopFreq,
                            int points,
                            double power,
                            int ifBw) override;

    void onSetupGraph(const QVector<QPointF>& data,
                      double startGhz,
                      double stopGhz) override;


private slots:
    // "Measure" button handler.
    void onMeasureButtonClicked();

private:
    Ui::MainWindow *ui;
    QLineSeries *m_series = nullptr;
    QChart *m_chart = nullptr;
    QValueAxis *m_axisX = nullptr;
    QValueAxis *m_axisY = nullptr;

    // Styling the graph.
    void setupChart();
    // Dynamically update the X axis.
    void updateXAxisRange(double startFreq, double stopFreq);

};

#endif // MAINWINDOW_H
