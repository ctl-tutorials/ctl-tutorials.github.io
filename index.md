# [CTL](https://gitlab.com/tpfeiffe/ctl) Tutorials

## Tutorial 1 - First steps

|Contents                 |                                                                  |
|-------------------------|------------------------------------------------------------------|
|General overview  	      | “From zero to first projections” & the module structure          |
|Projector extensions I   | the `StandardPipeline`, Poisson noise & polychromatic projections|
|Spectral volumes I       | how to create `SpectralVolumeData` using its factory methods     |
|Convenience/usability I  | using the file IO & finding information in the documentation     |
|CTL GUI I                | exploring `gui::plot()`                                          |


**By the end of this tutorial you will be able to:**
1. create a Qt project that uses CTL modules,
2. run simple simulations of 3D cone beam projections,
3. load and save data,
4. visualize volume and projection data, and
5. consider Poisson noise and a polychromatic spectrum in the simulations.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial1/)

[Video](https://youtu.be/3xp7uqrE1oE)


## Tutorial 2 - `CTL::MessageHandler` and GUI II

|Contents                 |                                                                  |
|-------------------------|------------------------------------------------------------------|
|Convenience/usability II | `CTL::MessageHandler` - customizing message output & log files   |
|CTL GUI II               | details on UI features, the property-based approach              |

**By the end of this tutorial you will be able to:**
1. use all features of (most) visualization widgets,
2. create fully-customizable visualizer objects, and
3. change the verbosity of message outputs and save a log file.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial2/)

[Video](https://youtu.be/4qrRYXtd_As)

## Tutorial 3 - The `Matrix` class template

|Contents                 |                                                                  |
|-------------------------|------------------------------------------------------------------|
|the `Matrix` class       | lightweight abstraction used for small matrices with fixed size  |

**By the end of this tutorial you will be able to:**
1. understand the purpose and design of the `Matrix`
2. work with the `Matrix` class efficiently with or w/o using the CTL, including
    - data access
    - operations
    - printing and visualization
3. find many mistakes already at compile time and prevent undefined behavior

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial3/)

[Video](https://youtu.be/Ej1j_cIYu18)

## Tutorial 4 - Details on `SpectralVolumeData` and `CompositeVolume`

|Contents                 |                                                                  |
|-------------------------|------------------------------------------------------------------|
|Spectral volumes II      | create SpectralVolumeData from existing VoxelVolume data         |
|Composite volumes I      | how to manually compose volumes                                  |
|Composite volumes II     | decompose volumes using `TwoMaterialThresholdVolumeDecomposer`   |

**By the end of this tutorial you will be able to:**
1. create spectral volumes from density and attenuation (incl. HU) data,
2. compose volumes consisting of multiple different sub-volumes, and
3. use a simple threshold decomposer to segment a volume into two materials.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial4/)

[Phantom data used](https://data.stimulate.ovgu.de/f/4f1a9a146c834af3ac52/?dl=1)

[Video](https://youtu.be/HrCkB7jzVTI)

## Tutorial 5 - `CTSystem`, `SimpleCTSystem`, and the `JSONSerializer`

|Contents                 |                                                                   |
|-------------------------|-------------------------------------------------------------------|
|CTSystem I			      | create, populate, and convert CTSystem and SimpleCTSystem objects |
|CTSystem II 		      | how to adapt systems created from blueprints to your needs        |
|Convenience/usability III|	de-/serialize systems and setups using JSONSerializer             |


**By the end of this tutorial you will be able to:**
1. create systems from individual components and inspect their properties,
2. adapt a system created from a blueprint to represent your requirements, and
3. use the JSONSerializer to easily save and load entire AcquisitionSetups.


[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial5/)

[Video](https://youtu.be/8O9pASZN8TU)

## Tutorial 6 [Special] - OpenCL helper classes

|Contents                 |                                                                   |
|-------------------------|-------------------------------------------------------------------|
|MWE "pure OpenCL"	      | the OpenCL C++ API: the princial "workflow" of an OCL program	  |
|OpenCL Config 		      | class that globally manages OpenCL objects (context, devices, ...)|
|PinnedMemory			  |	allows fast efficient data transfer between host and devices 	  |


[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial6/)

[Video - Part 1](https://youtu.be/VtTQZEOwaro)  
[Video - Part 2](https://youtu.be/qDZmy1rzHjk)


## Tutorial 7 - System Components

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|System components I	  | general overview and details about gantry, source, and detector classes |
|CTL Geometry I		      | basic concepts of geometry specifications in the CTL                    |

**By the end of this tutorial you will:**
1. understand the basic concepts behind the different system component types,
2. know the capabilities of built-in system components, and
3. have a general understanding of how geometrical information is specified in the CTL.

[Slides](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial7/tutorial7.pdf)

[Video](https://youtu.be/b_hsR4o4UQ4)  


## Tutorial 8 - `AcquisitionSetup`

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|AcquisitionSetup I	      | general overview of the `AcquisitionSetup` and `AcquisitionSetup::View` |
|AcquisitionSetup II      | strategies of modifying setups to your demands                          |

**By the end of this tutorial you will:**
1. understand how the `AcquisitionSetup` class works,
2. know how to use preparation protocols to adjust the setup, and
3. know how to manually construct an entire `AcquisitionSetup` for customized scenarios.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial8/)

[Slides](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial8/slides.pdf)

[Video](https://youtu.be/0BC0EcvcTXs)


## Tutorial 9 - Reconstruction

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|Reconstruction I	      | general overview of reconstruction in the CTL, `OCL::FDKReconstructor`  |
|Reconstruction II        | iterative reconstruction with `ARTReconstructor`                        |
|Reconstruction III       | special topics: polychromatic reconstruction, iterative FDK             |

**By the end of this tutorial you will:**
1. understand how the interface of reconstruction classes works in general,
2. be able to use the `FDKReconstructor` and `ARTReconstructor` classes, and
3. know how to use the `ARTReconstructor` class in more sophisticated scenarios.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial9/)

[Slides](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/Tutorial9/stoppingcrit/stoppingcrit.pdf)

[Video](https://youtu.be/OAXHxJ0a2jc)


## Tutorial 10 - Working with external/real data

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|Convenience/usability IV | reading/writing meta information using the `io::NrrdFileIO`             |
|Reconstruction IV	  | import, preparation, and reconstruction of external/real data           |

**By the end of this tutorial you will:**
1. be able to read and write meta information from/to files,
2. know how to read, prepare, and reconstruct (real) data from external sources.

(Source code) TBD

[Raw data](https://data.stimulate.ovgu.de/d/91b97fc822e94d2ab800/)

The example data of the flat detector setting (i.e. files fdct_xxx.den) are part of a collaboration with the group of Prof. Dr. Thorsten Walles from the University Clinic for Cardiac and Thoracic Surgery Magdeburg.

(Video) TBD


## Tutorial A1 - Sub-classing `AbstractCTSystemBlueprint`, `AbstractDataModel`, and `AbstractPreparationProtocol`

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|Custom content I         | overview of aspects in the CTL that can be extended by custom content   |
|Custom content II	      | creating own system blueprints, data models, and preparation protocols  |
|De-/serialization I	  | first steps towards getting custom classes de-/serializable             |

**By the end of this tutorial you will:**
1. understand the basic principles of how extending the CTL generally works (for most parts),
2. be able to implement you own system blueprints, data models, and preparation protocols,
3. know how to make your custom data models de-/serializable.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/TutorialA1/)

[Slides](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/TutorialA1/slides.pdf)

[Video](https://youtu.be/NqY1u4K_wUY)


## Tutorial A2 - Sub-classing `AbstractProjectionFilter` and `AbstractVolumeFilter`

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|Custom content III       | creating own projection and volume filters (CPU versions)               |
|De-/serialization II     | de-/serialization of types that contain poylmorphic (CTL) types         |

**By the end of this tutorial you will:**
1. be able to implement your own projection and volume filters,
2. know how to make custom data types that contain poylmorphic types de-/serializable.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/TutorialA2/)

[Video](https://youtu.be/DYfe3c4lV00)


## Tutorial A2B - Creating OpenCL-based filters with `GenericOCLProjectionFilter` and `GenericOCLVolumeFilter`

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|Custom content V         | creating OpenCL-based projection and volume filters                     |

**By the end of this tutorial you will:**
1. be able to use the `GenericOCLProjectionFilter` and `GenericOCLVolumeFilter` to implement projection and volume filters that utilize OpenCL.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/TutorialA2B/)

[Video] (TBD)


## Tutorial A3 - Implementing a custom file IO on the example of (pure) raw data

|Contents                 |                                                                         |
|-------------------------|-------------------------------------------------------------------------|
|Custom content IV        | implementing a custom file IO (read/write files of a particular type)   |

**By the end of this tutorial you will:**
1. be able to implement an IO class to read/write from/to files of a particular file type.

[Source code](https://github.com/ctl-tutorials/ctl-tutorials.github.io/blob/main/TutorialA3/)

[Raw data](https://data.stimulate.ovgu.de/d/6071644c69bb48c28712/)

[Video](https://youtu.be/ktzP0xMwfF0)
