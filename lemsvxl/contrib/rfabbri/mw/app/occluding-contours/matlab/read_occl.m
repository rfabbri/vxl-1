function [Gamma_center,Gamma_binormal,Gamma_radius] = read_occl()
% reads occluding contour (circle in 3D) for a desired camera (hardcoded bellow)
%
mdata=myread('ct-spheres/dat/Gamma-occl-params-1-cam01.dat');
Gamma_center = mdata(1:3);
Gamma_binormal = mdata(4:6);
Gamma_radius = mdata(7);
