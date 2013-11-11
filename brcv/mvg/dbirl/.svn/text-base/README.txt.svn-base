Brown Information-theoretic Registration Library

AUTHOR: Thomas Pollard
DATE: Oct 17 2005

SUMMARY: A library for registering images, given tracks on the ground 
plane in dbinfo_track_storage form.

CONTENTS:

dbirl_h_computation.h/.cxx
Main code for estimating homographies from the tracks

\tests
Testing code for the above files.

\pro
vidpro processes for the homography computation.  This needs some work
but it currently takes in a tracking file as input and outputs
the registration homographies.  Eventually it should be changed to 
take in tracking data right out of the tracking process, and used
with the image registration process found in dbvrl.  In the mean time
its best to use the following command line app.

\app
Here is the command-line application for registering video.  Usage is:

dbirl_app track_file image_in_dir image_out_dir homography_type ground_frame

where:

 -track_file is a file generated from a dbinfo_track_storage object.
 -image_in_dir is the directory of the unregistered images.
 -image_out_dir is the destination directory for the registered images.
 -homography_type is either "affine" or "similarity" for the type of
  homography to be computed in the registration.
 -ground_frame is an integer index of the frame in the image sequence 
  to which the other images images should be registered.
