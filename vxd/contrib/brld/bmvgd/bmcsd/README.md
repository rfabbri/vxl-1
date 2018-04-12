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


## Input dataset

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

The camera can be in at least two different formats:

##### Format 1: Intrinsic/Extrinsic pairs
In this format, the instrinsic parameters are the same for all images,
and is specified in a single file together with the images, called
'calib.intrinsic`.  This file contains a standard 3x3 matrix K as described in
Hartley & Zisserman, in ASCII format. Make sure you use double precision. Example:

```bash
$ cat calib.intrinsic

   2.2000000000000000e+03   0.0000000000000000e+00   6.4000000000000000e+02
   0.0000000000000000e+00   2.2000000000000000e+03   3.6000000000000000e+02
   0.0000000000000000e+00   0.0000000000000000e+00   1.0000000000000000e+00
```





## Credits

Copyright (c) [Ricardo Fabbri](http://rfabbri.github.io)
