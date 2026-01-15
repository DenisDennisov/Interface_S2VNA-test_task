// Abstract View interface.

#ifndef IVNAVIEW_H
#define IVNAVIEW_H

#include <QMainWindow>
#include <QVector>
#include <QObject>
#include <QtPlugin>


// Inherit from QMainWindow in an abstract class,
// to communicate with the Presenter signals.
class IView : public QMainWindow
{
    Q_OBJECT

public:
    explicit IView(QWidget *parent = nullptr)
        : QMainWindow(parent){}
    virtual ~IView() = default;

    // Change the connection status.
    virtual void onStatusUpdated(const QString& msg) = 0;

    // Updating data interface.
    virtual void onParamsUpdated(double startFreq,
                                    double stopFreq,
                                    int points,
                                    double power,
                                    int ifBw) = 0;

    // Update graph interface.
    virtual void onSetupGraph(const QVector<QPointF>& data,
                                double startFreq,
                                double stopFreq) = 0;

signals:
    // Button click signal to send data to Presenter.
    void measureRequested(double startFreq,
                            double stopFreq,
                            int points,
                            double power,
                            int ifBw);
};

Q_DECLARE_INTERFACE(IView, "Denis.Dennisov.TestTask/1.0")

#endif // IVNAVIEW_H
