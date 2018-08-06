milk2ds9
============

A simple utility to send images from the MILK shared memory structure to
the ds9 image viewer.

### Dependencies
You need:
- ds9 (http://ds9.si.edu/site/Home.html)
- XPA (http://hea-www.harvard.edu/RD/xpa/)

  Note: you need the header installed, I recommend just compiling it yourself instead of using the package manager.
- cfitsio (https://heasarc.gsfc.nasa.gov/docs/software/fitsio/fitsio.html)
- CACAO (https://github.com/cacao-org/cacao) or at least MILK (https://github.com/milk-org/milk).

### Building
Just use
```
g++ -O3 -o milk2ds9 milk2ds9.cpp -lxpa  -lpthread -I/path/to/ImageStreamIO/
```
where you set `/path/to/ImageStreamIO` to point to the directory containing `ImageStreamIO`.

Note that we don't link against cfitsio, that is in the dependencies just for some definitions in the header.

### Usage:

Usage: `./milk2ds9 [-h] [-f frameno] [-p pauseTime] [-s semaphoreNumber] [-t ds9Title] [-w waitTime] /path/to/filename`


Required Argument:

     `/path/to/filename`   the full path to the shared memory file.

Options:

     -h                 print help message and exit.  
     -f frameNo         specify the frame in which to display.
                        Default is 1.
     -p pauseTime       specify the time, in usec, to pause
                        before re-checking the semaphore.
                        Default is 100 usec.
     -s semaphoreNumber specify the semaphore number to monitor
     -t ds9Title        specify the title of the DS9 window to
                        use.  Default is the filename.
     -w waitTime        specify the time, in usec, to wait
                        after sending an image to DS9.  Default
                        is 1000 usec.