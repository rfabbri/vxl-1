function [] = test_hand_edge42()
%TEST
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: Sep, 2006

% debug switch
DEBUG = 1;
% parameters controlling the output
resultpath = '../result/hand_edge42/';
fname_pointcloud = 'pointcloud.txt';
fname_vrml_pointcloud = 'pointcloud.wrl';
fname_vrml_pointcloud_nointerp = 'pointcloud_nointerp.wrl';

% load the data
fprintf(1, 'Loading data and computing the edges with gradient and orientation ...\n');
data.imnum = 42; 
interval = round(670/data.imnum);
series = 0:interval:669;
datapath = '../data/hand/edgemaps/';
for i = 1:data.imnum
    fprintf(1,'[%d]',i);
    data.imnamecell{i} = [datapath 'edgemap_' num2str(series(i)) '.png'];
    im = imread(data.imnamecell{i});
    %[data.edgemap{i}, data.edgegradmap{i}, data.edgeormap{i}, data.edgeposmap{i}] = edgeGOPmap(im, 1, 0, 1, 10);
    edgebw = im2bw(im,0);
    edgebw(:,1:2) = 0; edgebw(:,end-1:end) = 0; edgebw(1:2,:) = 0; edgebw(end-1:end,:) = 0;
    edgemapraw = bwmorph(edgebw,'skel',Inf);
    [edgelist edgeim] = edgelink(edgemapraw, 20);
    [rownum, colnum] = size(edgemapraw);
    data.edgemap{i} = list2map(edgelist,rownum,colnum);
    data.edgegradmap{i} = sparse(double(im).*data.edgemap{i});
    data.edgeormap{i} = edgetangent(edgelist, rownum, colnum);
    
    if DEBUG
        imwrite(full(data.edgemap{i}), ['../debug/hand_edge42/' 'edgemap_' int2strfixed(i,4) '.png']);
    end
    angle = 2*pi*series(i)/670;
    data.imMcell{i} = loadpmatrix(angle);
end

data.silnum = 27;
silpath = '../data/hand/silhouettes/';
for i = 1:data.silnum
    data.silnamecell{i} = [silpath int2strfixed((i-1)*25,3) '-segmented.png'];
    angle = 2*pi*(i-1)*25/670;
    data.silMcell{i} = loadpmatrix(angle);
end

if DEBUG
    save('../debug/hand_edge42/programdata.mat');
end

[pts3drawcell, confrawcell] = struct_from_sil(data,1,10,'hand_edge42');

if DEBUG
    save('../debug/hand_edge42/programdata.mat');
end

maskcell = detectoutlier(pts3drawcell, confrawcell, data);
pts3dcell = removeoutlier(pts3drawcell, maskcell);
pts3d = cell2matrix(pts3dcell);

pts3dcell_nointerp = cell(data.imnum);
for i = 1:data.imnum
    pts3dcell_nointerp{i} = pts3dcell{i}(:,:,1);
end
pts3d_nointerp = cell2matrix(pts3dcell_nointerp);

if DEBUG
    save('../debug/hand_edge42/programdata.mat');
end

fid_text = fopen([resultpath fname_pointcloud], 'w+');
pctextwrite(fid_text, pts3d);
fclose(fid_text);

fid_vrmlpc = fopen([resultpath fname_vrml_pointcloud], 'w+');
pcvrmlwrite(fid_vrmlpc, pts3d);
fclose(fid_vrmlpc);

fid = fopen([resultpath fname_vrml_pointcloud_nointerp], 'w+');
pcvrmlwrite(fid, pts3d_nointerp);
fclose(fid);

function P = loadpmatrix(phi)
% functionality: load matrix calibration matrix after the rotation
% input:
%       - phi: rotation angle around y axis
% output:
%       - P: calibration matrix

% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: March 28, 2006

load('../data/hand/Calib_Results.mat');
load('../data/hand/base_extrinsics_rect.mat');

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
