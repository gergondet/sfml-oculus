sfml-oculus
===========

This package provides a library (libsfml-oculus) to use SFML inside an Oculus
RIFT. A simple example is also provided.

The idea is to have a virtual screen in the HMD where all drawings are done by
SFML. By default, the screen is setup to be seen entirely in the HMD but the
position of the screen can be adjusted (as shown in the example). 

The window that is displayed in the HMD is 640x480 up-scaled for rendering in
the HMD (the up-scaling factor can be accessed by getRenderScale() if needed)

=============
External dependencies :

- Oculus SDK (available from their website)

- Boost (filesystem)

- SFML (Simple and Fast Multimedia Library)
http://www.sfml-dev.org/ (version >=2.0)
