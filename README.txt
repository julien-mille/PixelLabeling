==================
Build instructions
==================

Requirements:
- OpenCV 3.x or 4.x
- Qt 5.x

The Makefile assumes that OpenCV 4 is installed in /usr/local/opencv-4.1.0-bin,
Qt headers in /usr/include/x86_64-linux-gnu/qt5, and Qt libraries in one of the standard 
lib directories.

Just edit the paths in the Makefile and run
> make
> ./pixelLabeling

======
Manual
======
Once an image is open, just paint pixel labels with the brush

Left mouse button pressed: label pixels with selected class inside the brush circle
Right mouse button pressed: unlabel pixels belonging to selected class inside the brush circle 
Delete: unlabel all pixels belonging to selected class
Shift: force mode. If the SHIFT key is pressed while labeling or unlabeling, all pixels are labeled or unlabeled, regardless
   of the class which they belong to...
...so Shift+Delete unlabels ALL pixels
F1/F2: decrease/increase the opacity of label colors
F3/F4: decrease/increase the diameter of the brush
A/Z: shortcuts for previous/next class

When the label image is saved, two files are generated:
  myimage_label.png: 8-bit image storing labels (this file can be open again later, using the "Open label image..." menu item)
  myimage_label_RGB.png: 24-bit RGB image storing label colors 

Please report any bug to julien.mille@insa-cvl.fr
Thanks!

Copyright 2015-2010 Julien Mille
