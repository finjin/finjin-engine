# finjin-engine
The Finjin engine is a multiplatform 3D simulation engine suitable for use in the creation of games and other interactive 3D applications.

## Features
  * Natively supports Windows (Win32 and UWP), with support for other desktop and mobile operating systems coming soon.
  * Driven by a multithreaded job and rendering system that automatically scales work across all CPU cores.
  * Utilizes the newest 3D graphics interfaces for improved graphics performance.
  * Supports multiple windows across multiple monitors.
  * Accepts inputs from a variety of input devices using a configuraton-driven input binding system.
  
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
LICENSE.txt               License file.
README.md                 This file.
configure.cfg             Source code configuration file. Modify this by hand when you start development for the very first time.
configure.py              Configuration script. Run this once  when you start development for the very first time.
config/                   Engine/runtime appication configuration files.
cpp/library/              Main source tree.
  project/                IDE project files for targeting different platforms.
  src/finjin/engine/      Implementation and include files.
    internal/             Platform-specific implementation.
      app/                Application, platform, and windowing implementation.
      gpu/                GPU/3D system implementation.
      input/              Input system implementation.
      sound/              Sound system implementation.
      vr/                 Virtual reality system implementation.
```
