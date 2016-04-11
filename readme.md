# Network UPS Tools Services for Windows

Network UPS Tools Services for Windows provides tools and services to run an `upsd` compatible
server to report the status of UPS devices connected to a system or server in master mode.  It
allows other Network UPS Tools slave daemons and monitoring clients access to UPS information using
standard Windows programing interfaces for easier setup.

## Installation

- Run the appropriate MSI for your processor platform.

## Building

### Requirements

- Visual Studio 2015 Community or better.
- Windows Installer XML Toolkit 3.9 or later (**for 3.9**, _the following is required_, similar to
  [Enable WiX project in Visual Studio 2013](http://stackoverflow.com/questions/19448343/enable-wix-project-in-visual-studio-2013))
    1. Copy _C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE\Extensions\Microsoft\WiX_
       to _C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\Extensions\Microsoft\WiX_
    2. Modify _C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\Extensions\Microsoft\WiX\extension.vsixmanifest_
       by adding the following:
       ```
       <VisualStudio Version="14.0">
           <Edition>Ultimate</Edition>
           <Edition>Premium</Edition>
           <Edition>Pro</Edition>
           <Edition>Express_All</Edition>
       </VisualStudio>
       ```
    3. Then open a Visual Studio 2015 Command Prompt such as _Developer Command Prompt for VS2015_
       and run `devenv /setup`.

## TODOs

In no particular priority.

- Add the SAL annotations to definitions so static code analysis may be used.
- Provides quicker service shutdown.
- Implement complete protocol.
- Create administration user interface.
- Add support for mulitple batteries.
- Add support for authentication.
- Proper installation instructions.

## License

Network UPS Tools Services for Windows is provided under the MIT license.  For more information see
the `LICENSE` file.
