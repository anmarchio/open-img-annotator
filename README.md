# Open Image Annotator

Open Image Annotator is a tiny application that allows you to quickly annotate images using superpixels and polygons.

## Open, Build and Run on Windows

### Build and Prepare OpenCV contrib

### Download and Configure wxWidgets

### Setting project references

_Note:_ Replace `<YOUR_VS_VERSION>` with the related VS Studio version you compiled OpenCV for. This can be `vc15` for `Visual Studio 2017` or `vc17` for `Visual Studio 2019`. For further details, see [#Build and Prepare OpenCV contrib]

- include `wxWidgetsX.X.X.props` in VS property window
- Add variable `OPENCVDIR` to environment variables, pointing to your opencv directory, e. g. `C:\OpenCV-3.5.4`
- add to your windows system path: `%OPENCVDIR%\opencv\build\install\x64\<YOUR_VS_VERSION>\bin`
- set include reference to point to `$(OPENCVDIR)\opencv\build\install\include`
- set lib reference to include `$(OPENCVDIR)\opencv\build\install\x64\<YOUR_VS_VERSION>\lib`