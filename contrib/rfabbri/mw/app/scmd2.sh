#!/bin/sh

set -x

#x..... write random cmds here... scmd -angle1 0 -angle2 5 -angle3 60


# General run on real data

# 1 - compute and store epipolar corresp
# 2 - stereo constraint
# 3 - compare output

#scmd -prefix /home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/david/david-02-26-2006-crop2/small-h100/
# -image1 000-maximage.png -image2 003-maximage.png -image3 006-maximage.png
# -edgels1 blba.edgels -edgels2 -edgels 3
#-trinocular
# -trinocular_nrad
#-compute_epipolars
# -compare_corresp




#./scmd -trinocular

#./scmd -compare_corresp -corresp_in david-0-3-edges_t10-stereo_result-trinocular-closest-sep15-104gt.corresp

#./scmd -compare_corresp -symmetric_3 -corresp_in synth-olympus-0-5-symmetric-noepitan-epipolar.corresp3 \
#  -prefix /home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/

#./scmd -trinocular_nrad 10 -angle1 0 -angle2 5 -angle3 60 -symmetric_3 -synthetic_2 -compare_corresp -prefix /home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/ -corresp_in synth-olympus-0-5-symmetric-noepitan-epipolar.corresp3 


#-------------------------
# May 9 08 SYNTHETIC TRINOCULAR
# 1 - generate edgels for synthetic
#
#  scmd -synthetic_med -angles "30,60,90" -synth_write_edgels \
#    -prefix /home/rfabbri/work/may9 \
#    -edgels '030deg-noepitan-digicam_table3.edgels 060deg-noepitan-digicam_table3.edgels 090deg-noepitan-digicam_table3.edgels'
#
# 2 - load them into breye
# 2.1 - save repository
#
# 2.2 - run epipolar triplets
#
#./scmd -compute_epipolars -symmetric_n -synthetic_med -angles "30,60,90" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#
# 3 - run stereo, generating corresp3
#
#./scmd -compute_epipolars -symmetric_3 -synthetic_med -trinocular -angles "30,60,90" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#
#
# 4 - open corresp in gui
# 5 - save repository
# 

#-------------------------
# May 9 08 STEEPLE TRINOCULAR
# 1 - generate edgels 
# 2 - load them into breye
# 2.1 - save repository
#
# 2.2 - run epipolar triplets
#
#./scmd -compute_epipolars -symmetric_3 -err_pos 0.1 -edgels 'frame_00000.edg frame_00050.edg frame_00100.edg' -edgel_type EDG -images 'frame_00000.png frame_00050.png frame_00100.png' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/work/may9/ -out_path /home/rfabbri/tmp/
#
# Try pairs just to see how many
#
#./scmd -compute_epipolars -symmetric_n -err_pos 1 -edgels 'frame_00000.edg frame_00050.edg' -edgel_type EDG -images 'frame_00000.png frame_00050.png' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/work/may9/ -out_path /home/rfabbri/tmp/
#
# 3 - run stereo, generating corresp3
#
#./scmd -compute_epipolars -symmetric_3 -synthetic_med -trinocular -angles "30,60,90" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#
#
# 4 - open corresp in gui
# 5 - save repository
# 


#-------------------------
# MAY 18 08 SYNTHETIC N-VIEW INTEGRATED TANGENTIAL AND EPIPOLAR CONSTRAINTS FOR
# building ntuplets


# - build n-tuplets
#
#./scmd -compute_epipolars -symmetric_n -trinocular_tangent_diff -synthetic_3 -angles "30,60,90" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/


#-------------------------
# MAY 18 08 CAPITOL N-VIEW INTEGRATED TANGENTIAL AND EPIPOLAR CONSTRAINTS FOR
# building ntuplets

#./scmd -compute_epipolars -symmetric_n -trinocular_tangent_diff  -err_pos 1 -edgels 'frame_00000-10edg.edg frame_00050.edg frame_00100.edg frame_00260.edg frame_00025.edg' -edgel_type EDG -images 'frame_00000.png frame_00050.png frame_00100.png frame_00260.png frame_00025.png' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/work/may9/ -out_path /home/rfabbri/tmp/

#./scmd -compute_epipolars -symmetric_n -err_pos 2 -edgels 'frame_00000-10edg.edg frame_00050.edg frame_00100.edg frame_00260.edg frame_00025.edg' -edgel_type EDG -images 'frame_00000.png frame_00050.png frame_00100.png  frame_00260.png frame_00025.png' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/work/may9/capitol/ -out_path /home/rfabbri/tmp/
 
#./scmd -compute_epipolars -symmetric_n -trinocular_tangent_diff -err_pos 1 -edgels 'frame_00000-10edg.edg frame_00050.edg frame_00100.edg' -edgel_type EDG -images 'frame_00000.png frame_00050.png frame_00100.png' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/work/may9/ -out_path /home/rfabbri/tmp/


# Short baseline:

#./scmd -compute_epipolars -symmetric_n -err_pos 2 -edgels 'frame_00000-10edg.edg frame_00001.edg frame_00002.edg frame_00003.edg' -edgel_type EDG -images 'frame_00000.png frame_00001.png frame_00002.png frame_00003.png' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/work/may9/capitol/ -out_path /home/rfabbri/tmp/

#./scmd -compute_epipolars -symmetric_n -trinocular_tangent_diff -err_pos 2 -edgels 'frame_00000-10edg.edg frame_00001.edg frame_00002.edg frame_00003.edg frame_00004.edg' -edgel_type EDG -images 'frame_00000.png frame_00001.png frame_00002.png frame_00003.png frame_00004.png' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/work/may9/ -out_path /home/rfabbri/tmp/



#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "0,30" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "30,0" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/

#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "0,60" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "60,0" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/

#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "60,30" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "30,60" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/

#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "0,30,60" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "60,0,30" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/

#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "0,30,60,90" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/
#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "90,0,30,60" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/

#./scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "0,10,30,45,60,75,80,90,105,130" -err_pos 0.001 -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/


# Generate edgels for synthetic:
scmd -synthetic_med -angles "0,30,60,90,120,240,270,300" -synth_write_edgels \
  -prefix /home/rfabbri/lib/data/synthetic-curves/med/edgels \
  -edgels '000deg-noepitan-digicam_table3.edgels 030deg-noepitan-digicam_table3.edgels 060deg-noepitan-digicam_table3.edgels 090deg-noepitan-digicam_table3.edgels 120deg-noepitan-digicam_table3.edgels 240deg-noepitan-digicam_table3.edgels 270deg-noepitan-digicam_table3.edgels 300deg-noepitan-digicam_table3.edgels'


#./scmd -trinocular_nrad 10 -angle1 0 -angle2 5 -angle3 60 -symmetric_3 -synthetic_2 -compare_corresp -prefix /home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/dat/synthetic-stereo/ -corresp_in synth-olympus-0-5-symmetric-noepitan-epipolar.corresp3 



# Build n-tuplets satisfying multiview epipolar constraint using synthetic data
#./scmd -compute_epipolars -symmetric_n -err_pos 0.001 -synthetic_2 -angles "0,30,60,90"  -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/tmp/

# Build n-tuplets satisfying multiview epipolar constraint using REAL DATA
# out_path is optional
#./scmd -compute_epipolars -symmetric_n -err_pos 1 -edgels 'p1017376.edg p1017377.edg' -edgel_type EDG -images 'p1017376.jpg p1017377.jpg' -cam_type "intrinsic_extrinsic" -write_epip -corresp_epip tst.corresp -prefix /home/rfabbri/lib/data/pot/views/sherd1/set1/crop/grant/  -out_path /home/rfabbri/tmp/
