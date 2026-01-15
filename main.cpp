// Application entry point.
// The following parameters must be passed to the Presenter: View, Model, and ConfigModel.
// Presenter parameters are inherited from abstract interfaces in the Interfaces directory.

#include "View/MainWindow.h"
#include "Model/VnaScpiClient.h"
#include "Model/VnaConfig.h"
#include "Presenter/MeasurementPresenter.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("MyTestTask");

    VnaConfigModel config;
    MainWindow view;

    MeasurementPresenter presenter(&view, &config);

    // Show View.
    view.show();

    return app.exec();
}
