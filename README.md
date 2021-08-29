# Open Image Annotator

Open Image Annotator is a tiny application that allows you to quickly annotate images using superpixels and polygons.

## Open, Build and Run on Windows

### Build and Prepare OpenCV contrib

### Download and Configure wxWidgets

### Setting project references

_Note:_ Replace `<YOUR_VS_VERSION>` with the related VS Studio version you compiled OpenCV for. This can be `vc15` for `Visual Studio 2017` or `vc17` for `Visual Studio 2019`. For further details, see [#Build and Prepare OpenCV contrib]

#### Update Windows environment paths

- Add variable `OPENCVDIR` to environment variables, pointing to your opencv directory, e. g. `C:\OpenCV-3.5.4`
- add to your windows system path: `%OPENCVDIR%\opencv\build\install\x64\<YOUR_VS_VERSION>\bin`

#### Configure Visual Studio Solution

*If wxWidgets references have not been configured to your solution file, then manually add wxWidgets properties:*

- in VS, select `View > Further Windows > Property manager`, then right click on project and select `open existing properties`
- select `wxWidgetsX.X.X.props` from your local wxWidgets directory

*If OpenCV references have not been added to your solution file, then manually add OpenCV properties:*

- in VS, right click on project explorer, select `Properties`
- then select `VC++ Directories`
- to `Include Directories` add: `$(OPENCVDIR)\opencv\build\install\include`
- to `Library Directories` add: `$(OPENCVDIR)\opencv\build\install\x64\<YOUR_VS_VERSION>\lib`
- in `Linker > Input > Additional Dependencies` add the following string: `opencvXXXd-world.lib`; `XXX` should be replaced with your custom opencv version