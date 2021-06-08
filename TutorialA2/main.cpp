#include <QApplication>
#include <QRandomGenerator>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

#include "custommodels.h"
#include "customvolumefilters.h"
#include "customprojectionfilters.h"

using namespace CTL;

// helper functions
void testSerialization(AbstractVolumeFilter& filter, const VoxelVolume<float>& image);
void testSerialization(AbstractProjectionFilter& filter, const ProjectionData& image);

// implementations
void useProjectionFilter();
void useVolumeFilter(std::shared_ptr<CTL::AbstractVolumeFilter> volumeFilt);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(MessageHandler::qInstaller);
    MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    try {

        // projection filters
        useProjectionFilter();

        // volume filters
        auto filter = std::make_shared<MovingAverageFilter>();
        useVolumeFilter(filter);

        auto model = std::make_shared<QuadraticFunctionModel>(2.0f, 1.5f, 0.5f);
        auto filter2 = std::make_shared<ModelApplicationFilter>(model);
        useVolumeFilter(filter2);

    }  catch (std::exception& err) {
        qCritical() << err.what();
    }

    return a.exec();
}

void useProjectionFilter()
{
    ProjectionData projections(500, 500, 1);
    projections.allocateMemory(10);
    projections.fill(1.0f);
    gui::plot(projections);

    MaskingFilter mask(QRect(100, 300, 300, 100));
    mask.filter(projections);
    gui::plot(projections);

    LinearExtrapolationFilter extrapolate(QRect(100, 300, 300, 100));
    extrapolate.filter(projections);
    gui::plot(projections);

    // serialization tests
    projections.fill(1.0f);
    testSerialization(mask, projections);
    testSerialization(extrapolate, projections);

}

void useVolumeFilter(std::shared_ptr<CTL::AbstractVolumeFilter> volumeFilt)
{
    auto volume = VoxelVolume<float>::cube(100, 1.0f, 0.0f);
    std::generate(volume.begin(), volume.end(),
                  [] { return QRandomGenerator::global()->bounded(1.0f); });

    // plot initial volume
    gui::plot(volume);

    // apply filter once
    volumeFilt->filter(volume);
    gui::plot(volume);

    // apply filter another time
    volumeFilt->filter(volume);
    gui::plot(volume);

    // serialization tests
    testSerialization(*volumeFilt, volume);
}





// ###################
// ##### HELPER ######

void testSerialization(AbstractProjectionFilter& filter, const ProjectionData& image)
{
    JsonSerializer ser;
    ser.serialize(filter, "temp.json");

    const auto deserializedFilter = ser.deserialize<AbstractProjectionFilter>("temp.json");

    auto imageCopy1 = image;
    filter.filter(imageCopy1);

    auto imageCopy2 = image;
    deserializedFilter->filter(imageCopy2);

    qInfo() << "Difference: " << metric::RMSE(imageCopy1.cbegin(), imageCopy1.cend(), imageCopy2.cbegin());
}

void testSerialization(AbstractVolumeFilter& filter, const VoxelVolume<float>& image)
{
    JsonSerializer ser;
    ser.serialize(filter, "temp.json");

    const auto deserializedFilter = ser.deserialize<AbstractVolumeFilter>("temp.json");


    auto imageCopy1 = image;
    filter.filter(imageCopy1);

    auto imageCopy2 = image;
    deserializedFilter->filter(imageCopy2);

    qInfo() << "Difference: " << metric::RMSE(imageCopy1.cbegin(), imageCopy1.cend(), imageCopy2.cbegin());
}
