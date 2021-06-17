#include <QApplication>
#include <QProcess>
#include <QRandomGenerator>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

#include "customvolumefilters.h"
#include "customoclvolumefilters.h"

// helper functions
void testSerialization(CTL::AbstractVolumeFilter& filter, const CTL::VoxelVolume<float>& image);
void testSerialization(CTL::AbstractProjectionFilter& filter, const CTL::ProjectionData& image);
void useProjectionFilter(std::shared_ptr<CTL::AbstractProjectionFilter> projFilt);
void useVolumeFilter(std::shared_ptr<CTL::AbstractVolumeFilter> volumeFilt);
std::shared_ptr<CTL::AbstractDataModel> piecewiseConstantModel();

// implementations
void tutorialA2B_1();
void tutorialA2B_2();


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(CTL::MessageHandler::qInstaller);
    CTL::MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    try {

        tutorialA2B_1();
        tutorialA2B_2();

    }  catch (std::exception& err) {
        qCritical() << err.what();
    } 

    return a.exec();
}

void tutorialA2B_1()
{
    // CPU version (see Tutorial A2's MaskingFilter)

    // GPU version
    const auto clFileName = std::string("F:/projects/ctl-tutorials/tutorialA2B/projectionmaskingfilter.cl");
    auto filter = std::make_shared<CTL::OCL::GenericOCLProjectionFilter>(clFileName, std::vector<float>{100.0f, 300.0f, 400.0f, 400.0f});
    useProjectionFilter(filter);
}

void tutorialA2B_2()
{
    // CPU version (using Tutorial A2's ModelApplicationFilter)
    auto segmentationModel = piecewiseConstantModel();
    CTL::gui::plot(segmentationModel);
    auto filter = std::make_shared<ModelApplicationFilter>(segmentationModel);
    useVolumeFilter(filter);

    // GPU version 1: fixed number of three thresholds
    const auto clFileName = std::string("F:/projects/ctl-tutorials/tutorialA2B/volumesegmentationfilter.cl");
    auto filter2 = std::make_shared<CTL::OCL::GenericOCLVolumeFilter>(clFileName, std::vector<float>{0.2f, 0.9f, 1.0f});
    useVolumeFilter(filter2);

    // GPU version 2: flexible number of thresholds
    const auto filter3 = std::make_shared<VolumeSegmentationFilter>(std::vector<float>{0.1f, 0.25f, 0.5f, 0.9f, 1.0f});
    useVolumeFilter(filter3);

}


// ###################
// ##### HELPER ######

std::shared_ptr<CTL::AbstractDataModel> piecewiseConstantModel()
{
    std::vector<std::shared_ptr<CTL::RectFunctionModel>> steps;
    steps.push_back(std::make_shared<CTL::RectFunctionModel>(0.0, 0.2, 0.0));
    steps.push_back(std::make_shared<CTL::RectFunctionModel>(0.2, 0.9, 1.0));
    steps.push_back(std::make_shared<CTL::RectFunctionModel>(0.9, 1.0, 2.0));

    std::shared_ptr<CTL::AbstractDataModel> start = std::make_shared<CTL::ConstantModel>(0.0f);

    return std::accumulate(steps.begin(), steps.end(), start);
}

void useVolumeFilter(std::shared_ptr<CTL::AbstractVolumeFilter> volumeFilt)
{
    auto volume = CTL::VoxelVolume<float>::cube(100, 1.0f, 0.0f);
    std::generate(volume.begin(), volume.end(),
                  [] { return QRandomGenerator::global()->bounded(1.0f); });
    CTL::gui::plot(volume);

    volumeFilt->filter(volume);
    CTL::gui::plot(volume);

    testSerialization(*volumeFilt, volume);
}

void useProjectionFilter(std::shared_ptr<CTL::AbstractProjectionFilter> projFilt)
{
    CTL::ProjectionData projections(500, 500, 1);
    projections.allocateMemory(10);
    projections.fill(1.0f);
    CTL::gui::plot(projections);

    projFilt->filter(projections);
    CTL::gui::plot(projections);

    projections.fill(1.0f);
    testSerialization(*projFilt, projections);
}

void testSerialization(CTL::AbstractProjectionFilter& filter, const CTL::ProjectionData& image)
{
    CTL::JsonSerializer ser;
    ser.serialize(filter, "temp.json");

    const auto deserializedFilter = ser.deserialize<CTL::AbstractProjectionFilter>("temp.json");

    auto imageCopy1 = image;
    filter.filter(imageCopy1);

    auto imageCopy2 = image;
    deserializedFilter->filter(imageCopy2);

    qInfo() << "Difference: " << CTL::metric::RMSE(imageCopy1.cbegin(), imageCopy1.cend(), imageCopy2.cbegin());
}

void testSerialization(CTL::AbstractVolumeFilter& filter, const CTL::VoxelVolume<float>& image)
{
    CTL::JsonSerializer ser;
    ser.serialize(filter, "temp.json");

    const auto deserializedFilter = ser.deserialize<CTL::AbstractVolumeFilter>("temp.json");


    auto imageCopy1 = image;
    filter.filter(imageCopy1);

    auto imageCopy2 = image;
    deserializedFilter->filter(imageCopy2);

    qInfo() << "Difference: " << CTL::metric::RMSE(imageCopy1.cbegin(), imageCopy1.cend(), imageCopy2.cbegin());
}
