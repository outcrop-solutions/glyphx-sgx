# GlyphX Build Instructions

## Prerequisite installs:

[QT 6.4.2 Installer](https://glyphx-build-tools.s3.us-east-2.amazonaws.com/qt-unified-windows-x64-4.5.1-online.exe)

[Visual Studio 2019 Community Installer](https://glyphx-build-tools.s3.us-east-2.amazonaws.com/vs_community__11d1cef4f41e4b1abc61bdf31105aa0a.exe)

[QT VS Add-In for 2019](https://glyphx-build-tools.s3.us-east-2.amazonaws.com/qt-vsaddin-msvc2019-2.10.1-rev.2.vsix)

[CMake Installer](https://glyphx-build-tools.s3.us-east-2.amazonaws.com/cmake-3.26.0-rc4-windows-x86_64.msi)

## Build Instructions:

- Starting with the Qt Installer
  1. Select Custom installation once getting to the Installation Folder page
  2. When selecting components, select the Qt drop-down then select the drop-down for Qt 6.4.2.
   
   - Check the follow:
      - MSVC 2019 64-bit
      - Addional Libraries:
        - Qt Positioning
        - Qt WebChannel
        - Qt WebEngine
        - Qt WebSockets
        - Qt WebView
  3. From here follow along with the installation prompts.
  4. Once installed set an env variable, QTDIR, to the msvc2019_64 directory path in the QT installation
    
    - Ex. C:\Qt\6.4.2\msvc2019_64
    
- The Visual Studio Installation is straightforward, just make sure to select "Desktop development with C++" under Desktop & Mobile from the Workloads tab.
 
- Next up is the QT VS Add-In. Visual Studio has to be closed when running this installer. Follow the prompts. Once installed, open Visual Studio 2019 and select the menu tab Extensions > QT VS Tools > QT Versions. Navigate to and select qmake.exe in `/msvc2019_64/bin` to add the new version. 

- Lastly, run the CMake installer and follow the prompts.
  
  - Once this is done open the CMake app:
    1. Set "Where is the source code" to the `/sgx/gui` directory 
    2. Set "Where to build the binaries" to the directory `/sgx/vs`. This directory may need to be created prior to running.
    3. Once those are set press "Configure" and select "Visual Studio 16 2019" as the generator for the project and then press "Finish"
    4. Assuming the QTDIR env variable is set properly as described above, the configuration should complete without error
    5. Lastly, press "Generate" to create the VS project files.
    
## Building & Running:

- Navigate to the directory `/sgx/3rdparty/scripts/` and run `setupWindowsOutputBin`. This will copy all the necessary dependencies to the output bin that VS will build the project files in.

- Open `GlyphBuilder.sln` located in `/sgx/vs/` to open the project in VS.

- Select either "Debug" or "Release" from the drop-down menu and set the Platform to "x64" right next to it. Then right-click GlyphX in the Solutions Explorer and select "Set as Startup Project"

- At this point you are ready to build and run the app!
