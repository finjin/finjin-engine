# finjin-engine
The Finjin engine is a multiplatform 3D simulation and visualization engine suitable for use in the creation of games and other interactive 3D applications.

## Features
  * Natively supports Windows (Win32 and UWP), Android, Linux, macOS, iOS, and tvOS.
  * Utilizes the newest 3D graphics interfaces (Direct3D 12, Vulkan, and Metal) for improved graphics performance.
  * Driven by a multithreaded job and rendering system that automatically scales work across all CPU cores.
  * Supports multiple windows across multiple monitors.
  * Accepts inputs from a variety of input devices using a configuration-driven input binding system.
  
## Mini FAQ
  * Is this project functional? Yes.
  * Is this project ready for production use? No.
  * When will this project be ready for production use? Sometime in 2017.
  
## Project Dependencies
* [finjin-common](https://github.com/finjin/finjin-common)

## Documentation
* [finjin-documentation](https://github.com/finjin/finjin-documentation)

## Repository Structure
```
LICENSE.txt                    License file.
README.md                      This file.
configure.cfg                  Source code configuration file. Modify this by hand when you start development for the very first time.
configure.py                   Configuration script. Run this once when you start development for the very first time.
config/                        Engine/runtime application configuration files.
cpp/library/                   Main source tree.
  project/                     IDE project files for targeting various platforms.
    android-codeworks-vs/      Android-specific project files.
    ios-xcode/                 iOS-specific project files.
    linux-netbeans/            Linux-specific project files.
    macos-xcode/               macOS-specific project files.
    tvos-xcode/                tvOS-specific project files.
    windows-uwp-vs/            Windows (UWP)-specific project files.  
    windows-win32-vs/          Windows (Win32)-specific project files.
  src/finjin/engine/           Implementation and include files.
    internal/                  Platform-specific implementation.
      app/                     Application, platform, and windowing implementation.
        android/               Android-specific implementation.
        ios/                   iOS/tvOS-specific implementation.
        linux/                 Linux-specific implementation.
        macos/                 macOS-specific implementation.
        uwp/                   Windows (UWP)-specific implementation.
        win32/                 Windows (Win32)-specific implementation.
      gpu/                     GPU/3D system implementation.
        d3d12/                 Direct3D 12-specific implementation. Used on Windows.
        metal/                 Metal-specific implementation. Used on Apple platforms.
        vulkan/                Vulkan-specific implementation. Used on Windows, Linux, and Android.
      input/                   Input system implementation.
        androidinput/          Android-specific implementation.
        gcinput/               Apple-specific game controller implementation.
        iosinput/              iOS/tvOS-specific implementation.
        linuxinput/            Linux-specific implementation.
        macosinput/            macOS-specific implementation.
        uwpinput/              Windows (UWP)-specific implementation.
        win32input/            Windows (Win32)-specific implementation.
        xinput/                Windows-specific game controller implementation.
      sound/                   Sound system implementation.
        avaudioengine/         AVAudioEngine-specific implementation. Used on Apple platforms.
        openal/                OpenAL-specific implementation. Used on Linux.
        opensles/              OpenSL ES-specific implementation. Used on Android.
        xaudio2/               XAudio2-specific implementation. Used on Windows.
      vr/                      Virtual reality system implementation.
        openvr/                OpenVR-specific implementation.
java/android/library/          Java/Android source tree.
tools/                         Various utility scripts.
```
