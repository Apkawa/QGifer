Download requirements
=============

* [mingw-get-setup.exe](http://heanet.dl.sourceforge.net/project/mingw/Installer/mingw-get-setup.exe)
* [qt-opensource-windows-x86-mingw482-4.8.6-1.exe](http://master.qt.io/archive/qt/4.8/4.8.6/qt-opensource-windows-x86-mingw482-4.8.6-1.exe)
* [cmake-3.2.2-win32-x86.exe](http://www.cmake.org/files/v3.2/cmake-3.2.2-win32-x86.exe)
* [opencv-2.4.11.exe](http://cznic.dl.sourceforge.net/project/opencvlibrary/opencv-win/2.4.11/opencv-2.4.11.exe)
* [giflib-4.1.4-1.exe](http://downloads.sourceforge.net/project/gnuwin32/giflib/4.1.4-1/giflib-4.1.4-1.exe)
* [nsis-3.0b1-setup.exe](http://netcologne.dl.sourceforge.net/project/nsis/NSIS%203%20Pre-release/3.0b1/nsis-3.0b1-setup.exe)

* [qt-creator-opensource-windows-x86-3.4.0.exe](http://master.qt.io/official_releases/qtcreator/3.4/3.4.0/qt-creator-opensource-windows-x86-3.4.0.exe)


Install requirements
=============

* Install **MinGW** to c:\MinGW. Append *C:\MinGW\bin* to **PATH**
* Install **Cmake**.
* Install **OpenCV**
    * Unpack opencv-2.4.11.exe to C:\.
    * Build MinGW binaries

        cd c:\opencv\build
        cmake c:\opencv\source -G "MinGW Makefiles"
        mingw32-make
        mingw32-make install

    * Append *C:\opencv\build\bin* to **PATH**


* Install Qt
* Install GifLib
* Install NSIS
