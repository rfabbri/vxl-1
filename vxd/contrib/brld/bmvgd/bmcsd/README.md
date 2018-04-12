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


# Running the system


## Input dataset

### Basic data: images
The input must be a sequence of images, for instance: 

```
frame_0000.png frame_0001.png frame_0002.png frame_0003.png frame_0004.png
frame_0005.png frame_0006.png frame_0007.png frame_0008.png frame_0009.png 
```

### Required derived data: cameras

Each image must have a camera model specifying both intrinsic and extrinsic
parameters ("fully calibrated"). These are allowed to be moderately
innacurate. Currently, camera models are estimated using a traditional
point-based structure from motion system such as Bundler (see also VisualSFM),
or manual calibration and pose estimation through specifying point or line
correspondences. We are in the process of developing fully curve-based
pipeline for determining the cameras, but this technology is not yet mature.



# Credits

Copyright (c) Ricardo Fabbri
