# 3D Curve Sketch, 3D Curve Drawing and Surfaced Multiview 3D Drawings

This documentation covers both the open source and the private parts of the
3D curve drawing-based system (which includes surface and albedo reconstruction).

As of this date, only the 3D curve sketch[1] is open sourced.
Whenever we refer to vxd or vxl, it is publically available online.
When we mention lemsvxl, this is a private repository you need explicit
permission to access.


## Introduction
This file contains instructions to run the system described in the website
http://multiview-3d-drawing.sf.net

## Hacking
For digging into the code, we recommend looking into the file
`introduction_doxy.txt`

## By using this code, you are required to cite the papers:

You must give proper credit for the privilege to access this software
system and its source code.

[1] 3D Curve Sketch: Flexible Curve-Based Stereo Reconstruction and Calibration,
CVPR 2010, R.Fabbri and B. Kimia. 

[2] From Multiview Image Curves to 3D Drawings, ECCV 2016, Ricardo Fabbri, Anil
Usumezbas and Benjamin Kimia, 

[3] The Surfacing of Multiview 3D Drawings via Lofting and Occlusion Reasoning, CVPR
2017, R. Fabbri, A. Usumezbas & B. Kimia

[4] Multiview Differential Geometry of Curves, R. Fabbri and B. Kimia, IJCV, 2016

[5] Camera Pose Estimation Using Curve Differential Geometry, IEEE Transactions
on Pattern Analysis and Machine Intelligence, 2020, Ricardo Fabbri, Peter J.
Giblin and Benjamin Kimi

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
point-based structure from motion system such as OpenMVG (see also Bundler,
VisualSFM, and Colmap), or by manual calibration and pose estimation through
specifying point or line correspondences. We are in the process of developing
fully curve-based pipeline for determining the cameras, but this technology is
not yet mature.

If your scene is modeled and rendered in CAD, eg, for ground truth comparison,
you can obtain camera models using Blender together with my python scripts:

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


-------------------------------------------------------------------------------
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

If you enable Boost in CMake, you can use a gzipped version of this `.edg.gz` for both
storage and speed.

### Computing subpixel edges

The third order detector from Amir Tamrakkar is used in the open source `vxd`,
and improvements from Yuliang Guo is used in the internal `lemsvxl`.
There are variants for color images that you may want to try in both the
open-source and internal variants. 

The basic classes are all open-sourced, located in `vxl/contrib/brl/bseg/sdet/*third*`,
with additional code in `vxd/contrib/brld/bsegd/sdetd`.

We, however, by default use the private classes in lemsvxl/*/dbdet which contain the
latest research code from the group, with improvements and bugfixes by us and by
Yuliang.

Edges are computed by a commandline script which internally uses the command
`dborl_edge_third_order` from the internal lemsvxl. This executable may use/link
to any version of the subpixel edge detector, but by default uses the internal/research one in
`lemsvxl/*/dbdet`.

Below is the basics of how to compute this using both GUI (recommended for
tuning the parameters) and commandline (for the actual batch processing. 

#### GUI
For an initial visual exploration of the edge detector's parameters,
you can start the GUI `sg`, load an image, and compute an edgemap.
Example
```
sg image.png
```
Use menu option `Processes > Edge Detection > Third Order Edge Detector`.
There may be variants for color iamges, but that is "premature optimization";
leave that for later. If you want to process an entire video, we recommend the
commandline option below. But you can also open the frames in the gui like so:

```
sg image-000.png image-001.png image-002.png   # and so on
```
or
```
sg *.png
```
Which will open one image per frame. You can navigate the frames using the arrow
keys. After detecting edges in the first frame, you can say `Process and Play
video`, which will repeat the last used process (edge detection) on each frame.

#### Commandline `edge` program

We provide a commandline program called `edge`, currently located in the internal
`lemsvxl/contrib/rfabbri/mw/scripts`. Example:
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
called `edge-scan` also from mw/scripts.

#### Details of the `edge` program

This command is implemented as a wrapper over `dborl_edge_third_order`,
whose source code is located at `lemsvxl/brcv/rec/dborl/algo/edge`. 

#### Most important parameters

You will want to detect edges for two purposes:

1) To be used for constructing curve fragments to be reconstructed
2) To be used as confirmation views to vote for 3D curve hypotheses

For purpose (1), you will want your edges to have very low threshold (high
recall), so that
geometric consistency be used as a robust filter by the symbolic edge linker /
curve fragment extractor below. This is to pick up faint but geometrically
salient constrast curves, for instance. Leaving the work for the goemetric
filter also has the advantage of not requiring you to tune the threshold - just
leave it very low.

For purpose (2), you will want less clutter. This can be tuned using
ground-truth validation experiments.

From experiments we found out two edge detector parameters are most relevant:
- sigma (neighborhod filter size)
- threshold (constrast)

The sigma parameter must be low to avoid localization error; we typically use 2 in
practice, sometimes 3 for noisy images. Our paper mentions we have used xx.

A typical low threshold for forming curve fragments is 0.2. You may increase
this in order to avoid the linking stage being too slow, but qualitatively this
fixed threshold has worked for many datasets for purpose (1) above.

For purpose (2), we suggest sigma at most 2, and a higher threshold. Our
datasets use:

-------------------------------------------------------------------------------
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
If you enable Boost in CMake, you can use a gzipped version of this `.cemv.gz` for both
storage and speed.

Note that this is different from a `.cem` file, which contains more linking
information. The original codebase confuses CEM and CEMV throughout the
codebase; so sometimes a `.cem` or a function claiming to work on CEM actually
expects CEMV-type data. The `V' in CEMV means CEM vsol. It is just a convention.
The process `dbdet_save_cem_process` saves CEM, while `vidpro1_save_cem_process`
will save it in the format required for the 3D Curve Sketch. This process is
accessed through the commandline or GUI as described below.

### Computing curve fragments

The symbolic edge linker Amir Tamrakkar is originally used and reside in the
public `vxd`. There are several
improvements by Yuliang Guo which are used by default but reside in the private
`lemsvxl`. There are variants for
color images and texture that you may want to try, but below is the basics of how to compute
this. 

The basic classes are all open-sourced, located in `vxl/contrib/brl/bseg/sdet/*symbolic*`,
with additional code in `vxd/contrib/brld/bsegd/sdetd`. 

By default we use private code in lemsvxl/*/dbdet (not the public one from vxd),
which is the latest research code we have for the edge linkers. But we can use
any of them.  The command `dborl_compute_contours`computes linked contours and
optionally computes curvelets. This command can be made to use any edge linker,
but by default it will use the research one in dbdet.


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

What you will see is a set of randomly colored curve fragments, which is
equivalent of the .CEM file format. You can extract only the curve fragments to
be used in the 3D Curve Sketch using `Proceses > Edge detection > Extract Linked
Curves`. The most useful parameters of this filter is the length threshold and
the `Smooth Extracted Contour > Number of Times`, leaving the stepsize 1. This
latter setting is for a simple yet effective curvature-based smoothing invented
by Ricardo Fabbri.

You can now save the edge map in the CEMV format required by the 3D curve sketch by
using `File > Save > .CEMv`. This will output the raw curve fragments with no
linking further information.

#### Commandline `contours` program

We provide a commandline utility called `contours`, currently located in
`lemsvxl/contrib/rfabbri/mw/scripts`. Example:
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

Note: The commandline program `curve` can also generate a more complete CEM file if desired, 
and will do so instead of CEMV based on the provided extension of the output
filename when writing to a file.

#### Parameter search
If you have a range of reasonable parameters set in the GUI that may work,
you can search for the best combination in parallel using a commandline script
called `contour-scan`.

#### Details of the `contours` program

This command is implemented as a wrapper over `dborl_compute_contours`,
whose source code is located at `lemsvxl/brcv/rec/dborl/algo/vox_compute_contours`. 
This performs the following steps: 
 (todo: describe)

#### Contours prost-processing

We can use machine learning and perform complex topological prost-processing on
top of the edgemaps, to get cleaner, well-connected curves. These are provided
by scripts `contour-break`, `contour-merge`, and `contour-rank`. We recommend
running them in the following order:

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
sg *.edg *.png *.cemv
```
Will open one (image,edg,fragment) triplet per frame.
If you want the same image for all frames, pass `-m`: 

```
sg *.edg *.png *.cemv -m
```

### Generate video of edges and curves from GUI

Generate images with OpenGL buffer content (for screenshots and video)
File → save as movie → multiple file codec

### Semi-manual curve fragment editing

We have a GUI editor to clean and topologically mend 2D curve fragments in case you want
to perform a semi-automated 3D reconstruction, or use these maps as ground truth
to train the merge/break/rank filters before applying to large datasets.
I have a file called `sel_gui-editor.odp` documenting this if need be.
The CEM visualizer (VXL tableau) has been extended to contain these commands:

```
select: Left click
Deselect: "Space"
Delete: "e"
Merge: "m"
Split: "s"
```

Merging is only applied if curves are close enough (3px);
Deletion is only applied if only one curve is selected (to prevent accidental deletion);
There is no "undo" action yet, be careful.

## Computing the 3D Curve Sketch and Drawing

- After edges and contours have been computed, we can run the multiview curve
  sketch `mcs` (under vxd/contrib/brld/bmvgd/bmcsd/cmd)[1] or the enhanced
  multiview curve sketch[2] `mcd` (under `mw/cmd`). The remaining of the pipeline is in
  Matlab


## Visualizing the 3D Curve Sketch


- Before compiling GUI code, which might be some work, you can use the command
    mca to print out in text format the number of curves in the reconstruction file 
- Compile sgui in mw/app
- Put mw/scripts/ in your path. This is how I do it: in your ~/.bashrc make sure you have:
```
PATH=$PATH:$HOME/bin/mw-scripts:$HOME/bin/mw-cmd:.
export PATH
```

Inside ~/bin you symlink to the script and cmd bin folders to have access to all
executables from the command line:
/home/rfabbri/bin/mw-scripts -> /home/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/scripts
/home/rfabbri/bin/mw-cmd -> /Users/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl-bin/contrib/rfabbri/mw/cmd

You need to have them in your path because you rarely will run just the curve
sketch binary or the dborl edge/contour binaries by hand.  You will run them
indirectly via scripts as explained in the bmcsd/README.md file


- You can now view all images and all edgemaps with typing sg * in the datset folder
- I used Matlab to look into the reconstructed curves, while anil uses
     Meshlab (I don't have access to his viewer). 
- some outlier curves might be floating so if you don't tune the parameters of
  the edge detector, linker and mcs matcher, then you might have a hard time
  seeing the recontruction because the outlier is so far away. You might want to
  zoom in greatly to see the actual object if your recontruction is "dirty".

### Matlab scripts

These reside in mw/app/matlab

In there, sexp/mcs_roc matlab script generates the ROC curves in CVPR'10.
After a curve sketch run, to read the curve sketch the commands are all in matlab/sexp/tracer (it is called tracer because the 3D curve sketch is actually an automated version of a GUI reconstruction tool to trace corresponding curves in multiple views and reconstruct them; my GUI still works, you can actually see which curves are matched and the reprojections to debug errors; the output of this GUI can also be visualized just the same with these tools).

To read the 3D curve sketch:
read_curve_sketch.m
At the same time, the supports for each curves will be a text file after running mcs or mcd, and this can be loaded as:
load supports
This can be used to experiment with pruning curves based on support or length scores inside matlab.
The input is the file after mcs/mcd is run, namely the file with this regexp: *-3dcurve-*-points*dat

To plot all the 3D curves:
plot_all_recs.m
This automatically also saves a .fig file in a file  'all_recs.fig', so if you see this file inside datasets, it means you can open the .fig in matlab,
and if you want access to the curves without rerunning the curve sketch, there is a utility to extract these curves from the plot back into a curve sketch
data structure.

To impose a bounding box and a good view angle for the paper,  for example, for the capitol sequence, for better visualization:
capitol_results.m

Other utiities are there that we can use to inspect the 3D curve sketch, prune short curves, to get back the curves from a saved .fig reconstruction,
another utility can be used to append / join two reconstrutions together (say join one for the overall capitol building, and another which is a zoom of the stairs)
(image-commands-util.sh).

mcs_instances.m is a utility to help building anchor and confirmation views using a more customized formula computed in matlab, if desired, instead of manually writing down the anchor and confirmation views.



## Multiview curve sketch attributes (`mca`)

## Credits

Copyright (c) [Ricardo Fabbri](http://rfabbri.github.io)
