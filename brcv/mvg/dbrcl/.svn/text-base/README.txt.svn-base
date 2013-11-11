Brown Registered Camera Library
Thomas Pollard, Nov '05

REMARK 3/7/06: Until further notice all files containing lists of cameras
or homographies will be required to be the output of a vpgl_list.  Older
lists from dbirl, dbvrl will be compatable.



WHAT IT IS:
  dbrcl is a library for computing camera matrices from registered video
without necessarily knowing any information about the cameras beforehand.
The user creates a constraint file containing constraints on two or more
cameras and the world, which may be made up if no information is known.  
Given a set of dbinfo tracks, the rest of the cameras are computed and
stored in dvpgl_proj_cameras.



HOW IT WORKS:
  The algorithms in this library require a video sequence that has already
been registered, preferably with dbirl to ensure accurate registration.  
Currently the algorithms require that the video focus on some common
region that is seen in all frames of the video, though this may be 
extended later.
  You will need to know two or more camera matrices from the sequence,
spaced well apart, preferably the first and last frames.  Since these
will most likely be unknown, the library provides code to compute these
two cameras from lists of constraints.  The two kinds of constraints used
are world-image correspondences and marked lines in the images that are
verticle in the world (assumed parallel to z axis).  The world-image 
correspondences are known 3d positions in the world and their pixel 
coordinates in the images, while the marked lines will be given by the 
pixel coordinates of their endpoints in the image (for example the top
and bottom of a building or telephone pole).  There must be enough of 
these constraints to compute the cameras, which means 5 or more 
world-image correspondences chosen such that some distance can be computed
from the points in the x,y,z directions.  The marked line constraints
only work to tighten the estimation of the world z-axis in the image.  The
primary way of inputing the constraints is a dbrcl constraint file, 
described later.
  It is worth remarking that these constraints can be made up as long
as they don't contradict each other.  For instance one can create a world
frame around a telephone pole in the image by making up 3d coordinates for
the top and bottom of the pole and using the road and crosswalks to 
find more 3d points that are all accurate up to scaling factors.  The 
cameras computed from this artificial frame will be fine for most 
purposes, though if any attempt at 3d reconstruction is made, the result
will be a (not-too-badly) warped version of the true world.
  Once the two or more known cameras are computed, we can compute the
rest of the cameras in the sequence using a set of dbinfo tracks that 
track points off of the registered plane across the whole video.  Such a 
track can be created using tools in dbinfo.  Though a single track is 
all that is needed for the computation, 5 or more are recommended for 
optimal results.



APPS:

dbrcl_camera_app
  This is an example app for using the dbrcl library and is for most 
purposes the only file you need to concern yourself with.  Its usage is:

  dbrcl_app constraint_file track_file camera_file
        
where:
-constraint_file is a dbrcl constraint file of form described later.
-track_file is a dbinfo_track_storage file of points tracked off of the
registered plane.
-camera_file is the file which will store the cameras after computation.


dbrcl_reproject_app
  Use this to verify your cameras are correct after computing them with
dbrcl_camera_app.  This projects the world points listed in a constraint
file into the images with the computed cameras.  If the points project
in the right place then you know the computation was successful. Usage is:

  dbrcl_reproject_app constraint_file camera_file image_in_dir 
    image_out_dir

where:
-constraint_file is a dbrcl constraint file.
-camera_file contains the cameras for the sequence.
-img_in_dir holds all of the images in the sequence which will only be
used to create marked images in img_out_dir to verify that the cameras
were computed correctly.
  

dbrcl_unregister_app
  If you've computed cameras for your registered sequence, and you still
have the homographies used to do the registration, you can use this app
to get the cameras for the unregistered sequence.  Usage is:

  dbrcl_unregister_app input_cameras input_homogs unregistered_images 
    output_cameras homog_spacing homog_start

where:
-input_cameras contains the cameras for the registered sequence.
-input_homogs contains the homographies used to register the video.
-unregistered_images is the directory of the unregistered images, which
is only used to determine the translation of the images in the registered
sequence (alternatively you can hack this app to input the translations
manually).
-output_cameras will hold the cameras for the unregistered sequence.
-homog_spacing and homog_spacing set which homographies from the 
homography file will be matched up with the cameras.  If you went straight
from computed cameras from the whole registered sequence than these should
be set to 1 an 0 respectively.  However if for example you deleted the 
first 100 frames from the registered sequence and then computed cameras
for only every 10th frame of the remaining sequence you should set these 
to 10 100, because the homog file will contain homogs for the whole
uncut sequence.



FILES:

dbrcl_compute_constrained_cameras: 
  These files contain classes for computing the two or more constrained 
cameras from lists of constraints.  The class "dbrcl_camera_constraints" 
is a storage class for constraints on a single camera.  The class 
"dbrcl_compute_constrained_cameras" is the main computation class for
computing the constrained cameras, which are all computed together.

dbrcl_compute_cameras:
  Code for computing all of the other cameras given the two or more 
known cameras and tracks.

dbrcl_point_tracker:
  Unfinished and unused code for computing tracks across the sequence
using the KLT tracker rather than dbinfo.  While this is fully automatic
it is slower and doesn't gaurantee the tracks will last the whole 
sequence as required and also produces many error tracks.  There is 
corresponding unused code in dbrcl_compute_cameras to deal with these 
tracks.



HOW TO CREATE CONSTRAINT FILES:

The first line of the constraint file must be:

  dbrcl camera constraint file

Constraints are given for a frame at a time.  To begin listing constraints
for a new frame use:

  frame 
  47

for example.  All constraints listed after this will be for frame 47. The
format for a world image correspondence is:

  world image correspondence
  0 20 20
  951 611

where in this example the world point 0 20 20 is seen at pixel 951 611 in
the image.  To give the image endpoints of a vector pointing "up" in the 
world use:

  height
  1022 685
  1009 614

where the two pixel locations given are the bottom and top of the object.
Finally if you want to add a 3d point to the world which won't be used
in camera computation but will be included in visuallization purposes
use:

  world point
  100 20 20

where the 3d point is (100,20,20).

An example constraint file is provided in example_constraints.txt.  Be 
sure not to deviate from the formatting given above as the file parser
isn't very robust to additional spaces/newlines.  Outside of the format
blocks given above, the user is free to add whatever text they want to 
the file such as descriptions of the 3d points given. When reading a 
constraint file the program will display any errors from the parsing.
