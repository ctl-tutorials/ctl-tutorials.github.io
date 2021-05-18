#include <QApplication>
#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

/*
 * Tutorial 9 - ART Stopping Criteria
 */

// helper function for creating a disk phantom
CTL::VoxelVolume<float> createDisk(float offSet, float muValue)
{
    constexpr auto radius = 50.f;
    constexpr auto height = 20.f;
    constexpr auto voxSize = 0.5f;
    auto ret = CTL::VoxelVolume<float>::cylinderZ(radius, height, voxSize, muValue);
    ret.setVolumeOffset(0.f, 0.f, offSet);
    return ret;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(CTL::MessageHandler::qInstaller);
    CTL::MessageHandler::instance().blacklistMessageType(QtDebugMsg);
    CTL::MessageHandler::instance().whitelistClassOrFunction("ART");

    // define a phantom
    const auto phantom = CTL::CompositeVolume{ CTL::VoxelVolume<float>::ball(40.f, 0.5f,  0.4f),
                                               CTL::VoxelVolume<float>::ball(35.f, 0.5f, -0.4f),
                                               createDisk(-30.f, 0.1f),
                                               createDisk(  0.f, 0.2f),
                                               createDisk( 30.f, 0.3f) };

    //defining the setup
    constexpr auto nbViews = 100u;
    constexpr auto sourceToIsocenter = 750.;
    auto setup = CTL::AcquisitionSetup{
        CTL::makeCTSystem<CTL::blueprints::GenericCarmCT>(CTL::DetectorBinning::Binning4x4),
        nbViews };
    setup.applyPreparationProtocol(CTL::protocols::WobbleTrajectory{ sourceToIsocenter });
    //CTL::gui::plot(setup);

    // projecting the volume
    const auto projections = CTL::OCL::RayCasterProjector{}.configureAndProject(setup, phantom);
    //CTL::gui::plot(projections);

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // reconstruct
    // FDK
    //CTL::gui::plot(CTL::OCL::FDKReconstructor{}.configureAndReconstruct(setup, projections,
    //                                                                    CTL::VoxelVolume<float>{ 256u, 256u, 100u,
    //                                                                                             1.f, 1.f, 1.f }));
    // ART
    CTL::ARTReconstructor art;

    art.defaultSubsetGenerator().setFixedNumberOfSubsets(20);
    //art.setStoppingCriteria(CTL::ARTReconstructor::NoStoppingCriterion);
    art.setStoppingCriterionEnabled(CTL::ARTReconstructor::MaximumNbIterations, false);
    art.setMaxTime(12.f, false);
    art.setMinRelativeProjectionError(0.02f, false);

    art.setStoppingCriteria(CTL::ARTReconstructor::NormalEquationSatisfied);

    CTL::gui::plot(art.configureAndReconstruct(setup, projections,
                                               CTL::VoxelVolume<float>{ 256u, 256u, 100u,
                                                                        1.f, 1.f, 1.f }));


    return a.exec();
}
