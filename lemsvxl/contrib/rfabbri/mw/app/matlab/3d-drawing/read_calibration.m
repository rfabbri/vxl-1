function [K,R,T] = read_calibration(file)

fid = fopen(file,'r');

K = zeros(3,3);
R = zeros(3,3);
T = zeros(3,1);

K = fscanf(fid,'%g',[3 3]);
R = fscanf(fid,'%g',[3 3]);
T = fscanf(fid,'%g',[3 1]);

K = K';
R = R';
