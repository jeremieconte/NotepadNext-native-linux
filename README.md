# Notepad Next

![Build Notepad Next](https://github.com/dail8859/NotepadNext/workflows/Build%20Notepad%20Next/badge.svg)
[![Release](https://img.shields.io/github/v/release/jeremieconte/NotepadNext-native-linux?label=release&color=brightgreen)](https://github.com/jeremieconte/NotepadNext-native-linux/releases/latest)

A native **Debian/Ubuntu** build of NotepadNext, the open-source reimplementation of Notepad++.

This repository provides a build of NotepadNext compiled natively for Linux and packaged as a `.deb`. The release is **only available for Debian/Ubuntu (amd64)** — it does not target Windows, macOS, or other Linux distributions. See the [latest release](https://github.com/jeremieconte/NotepadNext-native-linux/releases/latest) for the installable package.

![NotepadNext running natively on Linux](/doc/screenshot_linux.png)

Though the application overall is stable and usable, it should not be considered safe for critically important work.

There are numerous bugs and half working implementations. Pull requests are greatly appreciated.

# Installation

> **Note**: This release only targets **Debian/Ubuntu (amd64)**. For other platforms (Windows, macOS, other Linux distributions), use the [upstream NotepadNext project](https://github.com/dail8859/NotepadNext).

Download the `.deb` package from the [latest release](https://github.com/jeremieconte/NotepadNext-native-linux/releases/latest) and install it with `apt`, which handles the dependencies automatically:

```bash
sudo apt install ./notepadnext-native_0.15.0_amd64.deb
```

Alternatively, using `dpkg` (then resolve any missing dependencies):

```bash
sudo dpkg -i notepadnext-native_0.15.0_amd64.deb
sudo apt --fix-broken install
```

To build from source on Ubuntu, see [`BUILD-UBUNTU-26.04.md`](BUILD-UBUNTU-26.04.md).

# Translations
Translations are contributed by the community. All translations are managed using Crowdin at `https://crowdin.com/project/notepadnext`. If there is a language missing you would like to contribute, feel free to start a discussion on Crowdin.

# Development
This fork is built natively on Debian/Ubuntu with GCC and Qt 6. Step-by-step build instructions are provided in [`BUILD-UBUNTU-26.04.md`](BUILD-UBUNTU-26.04.md).

If you are familiar with building C++ Qt desktop applications, this is as simple as opening `CMakeLists.txt` and building/running the project. A more detailed general guide is also available [here](/doc/Building.md).

For upstream cross-platform development (Windows/macOS/other Linux), refer to the [original NotepadNext repository](https://github.com/dail8859/NotepadNext).


# License
This code is released under the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.txt).
