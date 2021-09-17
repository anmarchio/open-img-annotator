# Open Image Annotator

Open Image Annotator is a tiny, open source application that allows you to quickly annotate images using superpixels and polygons.

## Open, Build and Run on Windows

Usually, OpenCV can be used by simply downloading and unzipping the regular precompiled OpenCV code. The application can refer to the OpenCV path via environment variables.
For `Open Image Annotator`, we need some extra modules that are combined in additional project files called `OpenCV contrib`. Since precompiled files are not provided from official sources, we need to compile and configure them ourselves. The following tutorial explains step by step how to get and prepare the files using standard software on your computer.
This may seem complicated at first, but feel free to add bash scripts to automate the process.

### Setting up OpenCV with Cmake GUI

We need two programs to get OpenCV running properly: `Cmake`, a tool that generates configuration files from the original source code, and some version of `MS Visual Studio` or a related software that allows to open a solution `.sln` file and compiles the project.

1. Get [OpenCV](https://github.com/Itseez/opencv) 
2. Get [Cmake](https://cmake.org/)
3. Open Cmake (cmake-gui)
 - Enter path in `Where is the source code:`, e. g. `C:\OpenCV-4.5.3\opencv\source`; this is where to unzip the original opencv
 - Enter path in `Where to build the binaries:`, e. g. `C:\OpenCV-4.5.3\opencv\build`
 - Click `configure` and choose `Visual Studio 2015`, then click finish
 - Then press `generate` and wait a few seconds; Cmake will now generate the a few configuration files.

4. Download [OpenCV contrib](https://github.com/Itseez/opencv_contrib)
5. *Adding EXTRA MODULES:* Set the variable `OPENCV_EXTRA_MODULES_PATH` in the long list to the proper path, e. g. `<opencv_contrib>/modules`
![image](https://cloud.githubusercontent.com/assets/6803791/15631765/86d41f4e-25ac-11e6-8e1a-4966d8d42a17.png)
6. Now press `configure` again, then `generate`

More instruction details can be found in [SSARCandy/opencv_with_cmake.md](https://gist.github.com/SSARCandy/fc960d8905330ac695e71e3f3807ce3d) and [opencv_contrib](https://github.com/Itseez/opencv_contrib#how-to-build-opencv-with-extra-modules).

7. Now open `OpenCV.sln` from `<YOUR-OPENCV-PATH>\build\`
5. Build the project using `Debug`:   
  ![image](https://cloud.githubusercontent.com/assets/6803791/15509134/03aa71ae-2205-11e6-94be-a1a73a4cd7fe.png)  
  - right on the project in the solution explorer (usually located in the right frame), select `Build`
  - then switch to `Release` mode and build it again
  - *NOTE:* both actions may take several minutes depending on your hardware performance.
  
#### Update Windows environment paths

1. Add variable `OPENCVDIR` to environment variables, pointing to your opencv directory, e. g. `C:\OpenCV-3.5.4`
2. Add to your windows system path: `%OPENCVDIR%\opencv\build\install\x64\<YOUR_VS_VERSION>\bin`
3. It is probably safe to say: rebooting your system will make sure the changes to environment variables will be available on your system.

### Download and Configure wxWidgets

### Setting project references

_Note:_ Replace `<YOUR_VS_VERSION>` with the related VS Studio version you compiled OpenCV for. This can be `vc15` for `Visual Studio 2017` or `vc17` for `Visual Studio 2019`. For further details, see [#Build and Prepare OpenCV contrib]

#### Configure Visual Studio Solution for wxWidgets

*If wxWidgets references have not been configured to your solution file, then manually add wxWidgets properties:*

- in VS, select `View > Further Windows > Property manager`, then right click on project and select `open existing properties`
- select `wxWidgetsX.X.X.props` from your local wxWidgets directory

*If OpenCV references have not been added to your solution file, then manually add OpenCV properties:*

- in VS, right click on project explorer, select `Properties`
- then select `VC++ Directories`
- to `Include Directories` add: `$(OPENCVDIR)\opencv\build\install\include`
- to `Library Directories` add: `$(OPENCVDIR)\opencv\build\install\x64\<YOUR_VS_VERSION>\lib`
- in `Linker > Input > Additional Dependencies` add the following string: `opencvXXXd-world.lib`; `XXX` should be replaced with your custom opencv version

#### Update Windows environment paths

- Add variable `WXWIN` to environment variables, pointing to your opencv directory, e. g. `C:\wxWidgets-3.1.4`

### Compile Open Img Annotator

1. Clone repository to your local drive and cd to it
1. Open the solution `open-img-annotator.sln` using `MS Visual Studio` or a related program
1. Select `Release` and click `Compile`
1. If the compilation process suceeds, you will find an executable file in `<YOUR-DRIVE>\open-img-annotator\x64\Release` which starts the application.
