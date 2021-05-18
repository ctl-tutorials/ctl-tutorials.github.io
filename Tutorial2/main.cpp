#include <QApplication>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

using namespace CTL;

void tutorial2a();
void tutorial2b();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    tutorial2a();
    tutorial2b();

    return a.exec();
}

void tutorial2a()
{
    // set the message handler to the CTL::MessageHandler
    qInstallMessageHandler(MessageHandler::qInstaller);
    
    // configure the message output
    MessageHandler::instance().toggleMessageOriginTag(true);
    //MessageHandler::instance().blacklistMessageType(QtDebugMsg);
    MessageHandler::instance().blacklistFile("tabulateddatamodel.cpp");
    MessageHandler::instance().blacklistClassOrFunction("CarmGantryParam");
    MessageHandler::instance().enforceLoggingOfBlacklistMsg();
    MessageHandler::instance().setLogFileName("tutorialLog.txt");

    // from tutorial1b()

    // volume data
    auto volume = VoxelVolume<float>::cylinderZ(50.0f, 50.0f, 0.5f, 0.02f);

    // create an AcquisitionSetup with a generic C-arm CT
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(DetectorBinning::Binning4x4));
    setup.setNbViews(10);

    // assign a trajectory
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(500.0));

    qInfo() << setup.system()->photonsPerPixelMean();
    setup.system()->source()->setFluxModifier(0.01);
    qInfo() << setup.system()->photonsPerPixelMean();

    // create a projector
    auto projector = makeProjector<StandardPipeline>();

    // connect signals from the projector notifier to the MessageHandler
    QObject::connect(projector->notifier(), &ProjectorNotifier::information,
                     &MessageHandler::instance(), &MessageHandler::messageFromSignal);
    QObject::connect(projector->notifier(), &ProjectorNotifier::projectionFinished,
                     [] (int proj) { MessageHandler::instance().messageFromSignal("Finished view " + QString::number(proj)); } );

    projector->configure(setup);
    auto projections = projector->project(volume);

    gui::plot(projections);

    // write the log file
    qInfo() << MessageHandler::instance().writeLogFile();
}


#include <QRandomGenerator>
#include <QtCharts/QLineSeries>
void tutorial2b()
{
    auto volume = VoxelVolume<float>::cylinderZ(50.0f, 50.0f, 0.5f, 0.02f);

    gui::plot(volume);

    XYDataSeries series;
    for(int i = 0; i < 100; ++i)
        series.append(float(i), float(QRandomGenerator::global()->bounded(0, 1000)));

    // use additional arguments in plot()
    gui::plot(series, "Index", "Random value", true);

    // property-based approach
    auto lineViewer = new gui::LineSeriesView;
    lineViewer->setData(series);

    // QWidget settings
    lineViewer->setAttribute(Qt::WA_DeleteOnClose); // optionally: delete at appropriate time
    lineViewer->setWindowTitle("Fancy title");
    lineViewer->resize(400,300);

    // LineSeriesView settings
    lineViewer->setRangeY(1.0, 1000.0);
    lineViewer->setLabelX("Index");
    //lineViewer->setLogAxisY(true);

    // QChart settings
    auto chart = lineViewer->chart();
    chart->setTitle("Chart title");
    chart->setTheme(QtCharts::QChart::ChartThemeDark);

    // plot another series
    XYDataSeries series2;
    for(int i = 0; i < 100; ++i)
        series2.append(float(i), float(QRandomGenerator::global()->bounded(0, 1000)));

    auto anotherSeries = new QtCharts::QLineSeries;
    anotherSeries->append(series2.data());
    chart->addSeries(anotherSeries);

    anotherSeries->attachAxis(chart->axes(Qt::Horizontal).first());
    anotherSeries->attachAxis(chart->axes(Qt::Vertical).first());
    //anotherSeries->attachAxis(chart->axes(Qt::Vertical).at(1)); // req. when using logaritm. y-axis

    lineViewer->show(); // required to actually show the widget on the screen
}
