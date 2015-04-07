How to run the demo (using QDEGSAC and SIFT feature to compute the fundamental matrix)

1. run SIFT_computeF_QDEGSAC

   You will be asked to select two images. There are three demo images in this folder named d2_1.jpg, d2_2.jpg, d2_3.jpg.
   Just pick up any two of these. These images are example of degenerated data dominant the matching.
 
   Then this program will run SIFT feature detector. It takes a while for the first time you run it. And it will
   save the result in *.mat file automatically and next time it will be much faster.

   After SIFT feature detection, program compute the matching and use QDEGSAC to select inliers and compute the 
   fundamental matrix.

   At last you will see a diagram showing a few bars. These bars has different colors. Blue means these inliers are from
   the dominant plane, red means they are from places not on the dominant plane. 

2. run viewer_colines

   This program draws some corresponding epipolar lines on two images, on which you can see the matched points in blue
   and red color. Blue dots are from dominant plane, red ones are from other planes.

3. run viewer

   Display two images, you can click on one of them, and the corresponding epipolar lines will be drawed based on the
   fundamental matrix computed in step 1. You can therefore verify the result on different planes.