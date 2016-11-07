#
#
# Some useful commands when dealing with a region of interest
#
#



# Adding a new crop/region of interest, in this example the 'stairs' of the
# capitol:

cd /home/rfabbri/work/capitol

mkdir -p stairs/all-frames

cd stairs

# this is where a working subset of interest goes:
mkdir subset 

# this is where all 500 croped frames and calibration and edges go
cd all-frames

# Edges are in reps as vsols. We don't work directly with all 500 frames.
# Instead, we extract those you need for a working 3- or 4-view by saving vsl
mkdir rep

# The raw frames with the images go in here
mkdir /home/rfabbri/lib/data/dec/CapitolSite500/stairs

ln -s /home/rfabbri/lib/data/dec/CapitolSite500/stairs images


# Now we're ready to crop. 

cd /home/rfabbri/lib/data/dec/CapitolSite500/video_grey/

# An initial estimate for the crop window was estimated using gimp and is
# refined by trying the following command several times and inspecting the
# results in Konqueror.
for i in *.png; do convert -crop 653x333+259+386 $i ../stairs/$i; done


# update the .origin for the calibration

cd /home/rfabbri/work/capitol/stairs/all-frames/calib-opt-500
for i in *.origin; do echo '259 386' > $i; done

# now use brown eyes to compute vsols using the all-in-one process, in batches
# of 50


# then select a working group of images, copy them to subset dir
# Now copy the cameras for each working image
cd /home/rfabbri/work/capitol/stairs/subset
for i in `noext *.png`; do cp ../all-frames/calib-opt-500/$i.{extrinsic,origin} .; done

# Get the vsols of the frames of interest by loading the repository and saving
# the vsol as vsl format.

# You can also look at some adjacent frame and make
# it a frame of interest by copying int into subsets. Usually you can find a
# frame near a frame of interest that has a better linked curve of interest.
