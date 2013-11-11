% This is viewpoint_evaluation_script.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 9, 2012

%face images
clear

%I = double(rgb2gray(imread('/home/firat/Downloads/face/face_v1.bmp'))); 
%I = double(rgb2gray(imread('/home/firat/Downloads/face/face_v2.bmp')));
%J = double(rgb2gray(imread('/home/firat/Downloads/face/face_v7.bmp')));

I = double(rgb2gray(imread('/home/firat/Downloads/frog_v1_v2/frog_v1.bmp')));
J = double(rgb2gray(imread('/home/firat/Downloads/frog_v1_v2/frog_v2.bmp')));
load /home/firat/Downloads/frog_v1_v2/frog_v1-v2_undist.mat

%load /home/firat/Downloads/face/face_v1-v2_undist.mat
%load /home/firat/Downloads/face/face_v2-v7_undist.mat

1
[S, mins1, maxs1, saddles1] = get_critical_points(I,2);
[S, mins2, maxs2, saddles2] = get_critical_points(J,2);
evaluate_viewpoint(size(I), size(J), {mins1, maxs1, saddles1}, {mins2, maxs2, saddles2}, final_correspMap, final_pMap, 1:-.2:0, 1, 'r');

2
[S, mins1, maxs1, saddles1] = get_critical_points(I,3);
[S, mins2, maxs2, saddles2] = get_critical_points(J,3);
evaluate_viewpoint(size(I), size(J), {mins1, maxs1, saddles1}, {mins2, maxs2, saddles2}, final_correspMap, final_pMap, 1:-.2:0, 1, 'g');

3
[S, mins1, maxs1, saddles1] = get_critical_points(I,4);
[S, mins2, maxs2, saddles2] = get_critical_points(J,4);
evaluate_viewpoint(size(I), size(J), {mins1, maxs1, saddles1}, {mins2, maxs2, saddles2}, final_correspMap, final_pMap, 1:-.2:0, 1, 'b');

4
[S, mins1, maxs1, saddles1] = get_multiscale_critical_points(I);
[S, mins2, maxs2, saddles2] = get_multiscale_critical_points(J);
evaluate_viewpoint(size(I), size(J), {mins1, maxs1, saddles1}, {mins2, maxs2, saddles2}, final_correspMap, final_pMap, 1:-.2:0, 1, 'k--');

5
[image, descriptors, locs1] = sift(I);
[image, descriptors, locs2] = sift(J);
evaluate_viewpoint(size(I), size(J), {locs1(:,[2 1])}, {locs2(:,[2 1])}, final_correspMap, final_pMap, 1:-.2:0, 1, 'm--');

legend('cp sigma = 2', 'cp sigma = 3', 'cp sigma = 4', 'multi-scale cp', 'SIFT');

