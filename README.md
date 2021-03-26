# ps4-ipi

PS4 IPI, short for Internal PKG Installer, is a PlayStation 4 utility homebrew app which will install PKG files from the HDD, making it possible to install PKG files using FTP.

It probably already exists in some way for PS4, but I mainly found this useful for myself so I decided to implement it.

Thanks to sleirsgoevy's firmware escape the homebrew is firmware-agnostic and works on both HEN and Mira.

## Usage

1. Install the PKG onto your PS4;
2. Enable FTP and upload the PKG you want to install to `/data`;
3. Run the app.

## IPI vs Debug Settings

IPI does have a disadvantage compared to Debug Settings. Because we're uploading our PKG files now to the HDD instead of having it stored on a USB, the HDD requires 2x the space. This is fine for small PKG installations, but if your PKG is very large, check with your system if you have enough space on the HDD x 2.

## Credits

- [flatz](https://github.com/flatz), for the bgft bonus code in his [PKG installation writeup](https://flatz.github.io/#bonus)
- [sleirsgoevy](https://github.com/sleirsgoevy), for the extremely useful [ps4-libjbc](https://github.com/sleirsgoevy/ps4-libjbc)
- [OSM](https://github.com/OSM-Made), for [PS4-Notify](https://github.com/OSM-Made/PS4-Notify) and the libjbc sprx implementation
- [OpenOrbis](https://github.com/OpenOrbis), for the [PS4 Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
