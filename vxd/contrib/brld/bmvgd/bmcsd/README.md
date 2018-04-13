# 3D Curve Sketch


## Introduction
This file contains instructions to run the system described in the website
http://multiview-3d-drawing.sf.net

## Hacking
For digging into the code, we recommend looking into the file
`introduction_doxy.txt`

## By using this code, you are required to cite the papers:

You must give proper credit for the privilege to access this software
system and its source code.

- 3D Curve Sketch: Flexible Curve-Based Stereo Reconstruction and Calibration, CVPR 2010, R.Fabbri and B. Kimia. 

- The Surfacing of Multiview 3D Drawings via Lofting and Occlusion Reasoning, CVPR
2017, R. Fabbri, A. Usumezbas & B. Kimia

- Multiview Differential Geometry of Curves, R. Fabbri and B. Kimia, IJCV, 2016

- Camera Pose Estimation Using Curve Differential Geometry, ECCV 2012, Firenze, Italy, R. Fabbri, P. J. Giblin & B. Kimia

## System requirements

The 3D Curve Sketch system requires a Unix-like system, such as GNU/Linux and
Mac OS. The main developer uses both Mac OS and GNU/Linux but originally
developed the system on a Gentoo Linux workstation. Many of his students and
collaborators use Ubuntu Linux. The system will run fastest under GNU/Linux.


## Input dataset

All that is required is a set of images and (possibly approximate) camera model
information, detailed below. Curves and edges are computed for each frame as
detailed below. Only then can the 3D curve sketch itself run.

### Basic data: images
The input must be a sequence of images, for instance: 

```
frame_0000.png frame_0001.png frame_0002.png frame_0003.png frame_0004.png
frame_0005.png frame_0006.png frame_0007.png frame_0008.png frame_0009.png 
```

### Required derived data: cameras


#### Camera requirements
Each image must have a camera model specifying both intrinsic and extrinsic
parameters ("fully calibrated"). These are allowed to be moderately
innacurate. 

#### How to obtain cameras
Currently, camera models are estimated using a traditional
point-based structure from motion system such as Bundler (see also VisualSFM),
or by manual calibration and pose estimation through specifying point or line
correspondences. We are in the process of developing fully curve-based
pipeline for determining the cameras, but this technology is not yet mature.

If your scene is modeled and rendered in CAD, you can obtain camera models using
Blender together with my python scripts:

* http://blender.stackexchange.com/questions/38009/3x4-camera-matrix-from-blender-camera
* http://blender.stackexchange.com/questions/40650/blender-camera-from-3x4-matrix
* Detailed instructions in: https://github.com/rfabbri/pavilion-multiview-3d-dataset

#### Camera format

The camera can be in at least two different formats.
The code for this is at `bmcsd_util::read_cam_anytype()`.

##### Format 1: Projection matrix per image

In this format, a 3x4 camera matrix in text format specified as a
`.projmatrix` file for each image. For instance, in the same folder as image
`frame_0005.png` there is a file `frame_0005.projmatrix`. Make sure to use
double precision when generating your text file.


##### Format 2: Intrinsic/Extrinsic pairs

##### Intrinsic parameters
In this format, the instrinsic parameters are the same for all images,
and is specified in a single file together with the images, called
'calib.intrinsic`.  This file contains a standard 3x3 matrix K as described in
Hartley & Zisserman, in ASCII format. Make sure you use double precision. Example:

```bash
cat calib.intrinsic

   2.2000000000000000e+03   0.0000000000000000e+00   6.4000000000000000e+02
   0.0000000000000000e+00   2.2000000000000000e+03   3.6000000000000000e+02
   0.0000000000000000e+00   0.0000000000000000e+00   1.0000000000000000e+00
```

##### Extrinsic parameters

The extrinsic parameters are specified per file, with an extension `.extrinsic`.
For instance, in the same folder as image `frame_0005.png` there is a file
`frame_0005.extrinsic`. Each `.extrinsic` file is a text file as follows:

```bash
cat frame_0005.extrinsic

-0.10343499811291564927 1.6295626415092911987e-15 -0.994636215490558806
0.11785340886738646105 -0.99295536999761169206 -0.012255905158036265595
-0.98762937136545203565 -0.11848895810541228146 0.10270633682191222802
1088.1156793830618881 275.07425100389514228 -113.15618865547817506
```

Where the first 3x3 numbers form the camera's rotation matrix, and the last line
forms the camera center (not the translation vector!). 


## Subpixel Edgemaps

### Edgemap format

The format is extension `.edg`, in ASCII, one per image file, and looks like:

```
# EDGE_MAP v3.0

# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Strength Uncer

WIDTH=1280
HEIGHT=720
EDGE_COUNT=114056


[6, 5]    5.17601 17.4736   [6.48193, 5.48969]   5.17601 17.4736 0.107256
[7, 5]    5.0873 17.5829   [6.66549, 5.09222]   5.0873 17.5829 0.128001
[15, 5]    5.5694 21.966   [15.4614, 5.43892]   5.5694 21.966 0.302899
[16, 5]    5.63831 21.6765   [15.6423, 5.25879]   5.63831 21.6765 0.301624
....
```

### Computing subpixel edges

The third order detector from Amir Tamrakkar is used. There are variants for
color images that you may want to try, but below is the basics of how to compute
this. The basic classes are all open-sourced, located in `vxl/contrib/brl/bseg/sdet/*third*`,
with additional code in `vxd/contrib/brld/bsegd/sdetd`.

#### GUI
For an initial visual exploration of the edge detector's parameters,
you can start the GUI `sg`, load an image, and compute an edgemap.
Example
``
sg image.png
``
Use menu option `Processes > Edge Detection > Third Order Edge Detector`.
There may be variants for color iamges, but that is "premature optimization";
leave that for later. 

#### Commandline `edge` program

We provide a commandline utility called `edge`. Example:
```
edge image.png
```
Will produce the file `image.edg`. You can then inspect both with the GUI
```
sg image.png imge.edg                            # I simply use    sg image*
```

We recommend compute it in parallel by installing GNU Parallel. You can then
compute the edges for all images of your dataset in parallel:
```
parallel edge ::: *.png
```

#### Parameter search
If you have a range of reasonable parameters set in the GUI that may work,
you can search for the best combination in parallel using a commandline script
called `edge-scan`.


## Curve fragment (linked edges) information

## Curve fragment format

The linked curve fragments may be obtained from a different edgemap than what is
used as confirmation. They are extension `.cemv`, one per image file, in ASCII,
and looks like:

```
# Format :
# Each contour block will consist of the following
# [BEGIN CONTOUR]
# EDGE_COUNT=num_of_edges
# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf 
# ...
# ...
# [END CONTOUR]

CONTOUR_COUNT=
TOTAL_EDGE_COUNT=
[BEGIN CONTOUR]
EDGE_COUNT=38
 [0, 0]  0.0  0.0  [4.74236, 7.69503]  0.0  0.0
 [0, 0]  0.0  0.0  [5.04857, 7.94679]  0.0  0.0
 [0, 0]  0.0  0.0  [5.36333, 8.15191]  0.0  0.0
 ....
```

Note that this is different from a `.cem` file, which contains more linking
information. 

### Computing curve fragments

The symbolic edge linker Amir Tamrakkar is originally used. There are several
improvements by Yuliang Guo which can also be used.  There are variants for
color images and texture that you may want to try, but below is the basics of how to compute
this. The basic classes are all open-sourced, located in `vxl/contrib/brl/bseg/sdet/*symbolic*`,
with additional code in `vxd/contrib/brld/bsegd/sdetd`.


#### GUI
For an initial visual exploration of the edge linker's parameters,
you can start the GUI `sg`, load an image and edgemap, and compute curve
fragments.

Example
``
sg image.png image.edg
``
(you can also only load the image and compute the edge detector on the GUI)

Use menu option `Processes > Edge Detection > Symbolic Edge Linker`.
There may be variants, but that is "premature optimization"; leave that for later. 

#### Commandline `contours` program

We provide a commandline utility called `contours`. Example:
```
contours image.png
```
Will produce the file `image.cemv`. You can then inspect both with the GUI
```
sg image.png imge.edg image.cemv                           # I simply use    sg image*
```

We recommend compute it in parallel by installing GNU Parallel. You can then
compute the edges for all images of your dataset in parallel:

Case 1) If you want to recompute the edgemaps from scratch (to possibly use
different parameters than what you used to generate the `.edg` above:
```
parallel contours ::: *.png
```

Case 2) If you want to reuse the edgemaps:
```
parallel contours ::: --edgesuffix ::: .edg ::: *.png
```

#### Parameter search
If you have a range of reasonable parameters set in the GUI that may work,
you can search for the best combination in parallel using a commandline script
called `contour-scan`.

#### Contours prost-processing

We can use machine learning and other prost-processing on top of the edgemaps,
to get cleaner, well-connected curves. Thesea are provided by scripts
`contour-break`, `contour-merge`, and `contour-rank`. We recommend running them
in the following order:

```
parallel contour-break ::: *.png
parallel contour-merge ::: *.png
parallel contour-rank ::: *.png
```

## Curvelet information (Optional)


## Visualizing edges, images and camera information

There is a GUI called `sg` (Stereo GUI) in the internal LEMSVXL repository at
`lemsvpe/lemsvxl/contrib/rfabbri/mw/scripts` which you can use to visualize these
results:

```
Sg *.edg *.png *.cemv
```
Will open one (image,edg,fragment) triplet per frame.
If you want the same image for all frames, pass `-m`: 

```
sg *.edg *.png *.cemv -m
```

## Credits

Copyright (c) [Ricardo Fabbri](http://rfabbri.github.io)
