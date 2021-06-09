#include <QApplication>

#include "ctl.h"
#include "ctl_qtgui.h"

#include "rawdataio.h"

// helper
void testSaveLoad(const CTL::ProjectionData& projections,
                  std::unique_ptr<CTL::io::AbstractProjectionDataIO> io);
void testSaveLoad(const CTL::ProjectionData& projections,
                  uint viewId,
                  std::unique_ptr<CTL::io::AbstractProjectionDataIO> io);
void testSaveLoad(const CTL::VoxelVolume<float>& volume,
                  std::unique_ptr<CTL::io::AbstractVolumeIO<float>> io);

// implementations
void tutorialA3_1();
void tutorialA3_2();

// ### NOTE ###
// change this path to the folder where you placed the downloaded example files!
// (please make sure to end the path with a slash)
const QString DATA_PATH = "F:/projects/ctl-tutorials/data/";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(CTL::MessageHandler::qInstaller);
    CTL::MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    try {

        tutorialA3_1();
        tutorialA3_2();


    }  catch (std::exception& err) {
        qCritical() << err.what();
    }

    return a.exec();
}


void tutorialA3_1()
{
    const auto io = CTL::io::BaseTypeIO<CTL::io::NrrdFileIO>();
    const QString projectionFile = DATA_PATH + "scandata.nrrd";
    const QString volumeFile = DATA_PATH + "volume.nrrd";

    // reading projections
    const auto proj = io.readProjections(projectionFile);
    CTL::gui::plot(proj);

    // reading volume data
    const auto volume = io.readVolume<float>(volumeFile);
    CTL::gui::plot(volume);

    // reading a single slice from a volume
    const auto slice = io.readSlice<float>(volumeFile, 20);
    CTL::gui::plot(slice);

    // test writing and reading
    auto P = CTL::ProjectionData(123, 45, 6); P.allocateMemory(7); P.fill(8.0f);
    const auto V = CTL::VoxelVolume<float>::cube(200, 1.0f, 1.337f);
    testSaveLoad(P, io.makeProjectionDataIO());
    testSaveLoad(P, 3, io.makeProjectionDataIO());
    testSaveLoad(V, io.makeVolumeIO<float>());
}

void tutorialA3_2()
{
    const auto io = CTL::io::BaseTypeIO<RawDataIO<670, 60, 290, ushort>>();
    const QString projectionFile = DATA_PATH + "scandata.bin";
    const QString volumeFile = DATA_PATH + "volume.bin";

    // reading projections
    const auto proj = io.readProjections(projectionFile);
    CTL::gui::plot(proj);

    // reading volume data
    const auto ioVol = CTL::io::BaseTypeIO<RawDataIO<200, 200, 200, float>>();
    const auto volume = ioVol.readVolume<float>(volumeFile);
    CTL::gui::plot(volume);

    // reading a single slice from a volume
    const auto slice = ioVol.readSlice<float>(volumeFile, 20);
    CTL::gui::plot(slice);

    // test writing and reading
    auto P = CTL::ProjectionData(123, 45, 6); P.allocateMemory(7); P.fill(8.0f);
    const auto V = CTL::VoxelVolume<float>::cube(200, 1.0f, 1.337f);
    testSaveLoad(P, io.makeProjectionDataIO());
    testSaveLoad(P, 3, io.makeProjectionDataIO());
    testSaveLoad(V, ioVol.makeVolumeIO<float>());
}

// ##############
// ### HELPER ###
// ##############

void testSaveLoad(const CTL::ProjectionData& projections,
                  std::unique_ptr<CTL::io::AbstractProjectionDataIO> io)
{
    io->write(projections, "temp.bin");
    const auto loadedData = io->readProjections("temp.bin");
    qInfo() << "Save/load projections - difference: " << CTL::metric::RMSE(projections.cbegin(), projections.cend(), loadedData.cbegin());
}

void testSaveLoad(const CTL::ProjectionData& projections,
                  uint viewId,
                  std::unique_ptr<CTL::io::AbstractProjectionDataIO> io)
{
    io->write(projections, "temp.bin");
    const auto loadedData = io->readSingleView("temp.bin", viewId);
    const auto& originalView = projections.view(viewId);
    qInfo() << "Save/load single view - difference: " << CTL::metric::RMSE(originalView.cbegin(), originalView.cend(), loadedData.cbegin());
}

void testSaveLoad(const CTL::VoxelVolume<float>& volume,
                  std::unique_ptr<CTL::io::AbstractVolumeIO<float>> io)
{
    io->write(volume, "temp.bin");
    const auto loadedData = io->readVolume("temp.bin");
    qInfo() << "Save/load volume - difference: " << CTL::metric::RMSE(volume.cbegin(), volume.cend(), loadedData.cbegin());
}



/*
 * Potential extensions
 * - leave out 3rd dimension -> determine from file size
 * - skip certain number of bytes (header) -> similar to ImageJ
 * - "shortcuts" when types are same (no need for std::transform)
 * - many consistency checks (e.g. matching sizes)
 * - default data type (similar to other IOs, always assume type from CTL base type, e.g. ProjectionData -> float)
 */
