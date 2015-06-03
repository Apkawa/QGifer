Download requirements
=============

* [i686-4.8.3-release-posix-dwarf-rt_v3-rev2.7z](http://netcologne.dl.sourceforge.net/project/mingw-w64/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.3/threads-posix/dwarf/i686-4.8.3-release-posix-dwarf-rt_v3-rev2.7z)
* [qt-opensource-windows-x86-mingw482-4.8.6-1.exe](http://master.qt.io/archive/qt/4.8/4.8.6/qt-opensource-windows-x86-mingw482-4.8.6-1.exe)
* [cmake-3.2.2-win32-x86.exe](http://www.cmake.org/files/v3.2/cmake-3.2.2-win32-x86.exe)
* [opencv-2.4.11.zip](http://softlayer-ams.dl.sourceforge.net/project/opencvlibrary/opencv-unix/2.4.11/opencv-2.4.11.zip)
* [giflib-4.1.4-1.exe](http://downloads.sourceforge.net/project/gnuwin32/giflib/4.1.4-1/giflib-4.1.4-1.exe)
* [nsis-3.0b1-setup.exe](http://netcologne.dl.sourceforge.net/project/nsis/NSIS%203%20Pre-release/3.0b1/nsis-3.0b1-setup.exe)
* [qt-creator-opensource-windows-x86-3.4.0.exe](http://master.qt.io/official_releases/qtcreator/3.4/3.4.0/qt-creator-opensource-windows-x86-3.4.0.exe)


Install requirements
=============

* Unpack **MinGW** to c:\MinGW. Append *C:\MinGW\bin* to **PATH**
* Install **Cmake**.
* Install **OpenCV**
    * Unpack to *C:\opencv\source*.
    * Create folder *C:\opencv\build*
    * Build MinGW binaries

        cd c:\opencv\build
        cmake c:\opencv\source -G "MinGW Makefiles"
        mingw32-make

    * Append *C:\opencv\build\bin* to **PATH**

* Install Qt
* Install GifLib
* Install NSIS (TODO for use)
