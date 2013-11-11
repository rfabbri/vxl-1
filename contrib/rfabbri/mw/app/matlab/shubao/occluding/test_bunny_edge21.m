%TEST
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: Sep, 2006

% debug switch
DEBUG = 1;
% parameters controlling the output
resultpath = '../result/bunny_edge21/';
fname_pointcloud = 'pointcloud.txt';
fname_vrml_pointcloud = 'pointcloud.wrl';

% load the data
fprintf(1, 'Loading data ...\n');
data.imnum = 20; data.silnum = 20;
interval = round(720/data.imnum);
series = 0:interval:719;
datapath = '../data/bunny/edgemaps/';
for i = 1:data.imnum
    data.imnamecell{i} = [datapath int2strfixed(series(i),3) '-segmented.png'];
    im = imread(data.imnamecell{i});
    [data.edgemap{i}, data.edgegradmap{i}, data.edgeormap{i}, data.edgeposmap{i}] = edgeGOPmap(im, 1, 0, 1, 10);
    if DEBUG
        imwrite(data.edgemap{i}, ['../debug/bunny_edge21/' 'edgemap_' int2strfixed(i,4) '.png']);
    end
    angle = 2*pi*series(i)/720;
    data.imMcell{i} = loadpmatrix(angle);
end
silpath = '../data/bunny/silhouettes/';
for i = 1:data.silnum
    data.silnamecell{i} = [datapath int2strfixed(series(i),3) '-segmented.png'];
    angle = 2*pi*series(i)/720;
    data.silMcell{i} = loadpmatrix(angle);
end


[pts3draw, confraw] = struct_from_sil(data);
mask = detectoutlier(pts3draw, confraw, data);
pts3d = removeoutlier(pts3draw, mask);


fid_text = fopen(fname_pointcloud, 'w+');
pctextwrite(fid_text, pts3d);
fclose(fid_text);

fid_vrmlpc = fopen(fname_vrml_pointcloud, 'w+');
pcvrmlwrite(fid_vrmlpc, pts3d);
fclose(fid_vrmlpc);


function P = loadpmatrix(phi)
% functionality: load matrix calibration matrix after the rotation
% input:
%       - phi: rotation angle around y axis
% output:
%       - P: calibration matrix

% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: March 28, 2006

load('../data/bunny/Calib_Results.mat');
load('../data/bunny/base_extrinsics_rect.mat');

Mint = [fc(1) 0 cc(1)-crop_rect(1,1); 
                0 fc(2) cc(2)-crop_rect(2,1);
                0 0 1];
% For the calculation of Ry, refer to Professor Mundy's lecture notes.
Ry = [cos(phi) 0 -sin(phi);
        0 1 0;
        sin(phi) 0 cos(phi)];   
% For the update of R, P_c = R*P_w + t; then new P_c' = Ry*P_c = Ry*R*P_w + Ry*t.
R = Rckk*Ry;
T = Tckk;       % change the location of the world coordinate; a trick
Mext = [R T];
P = Mint*Mext;
