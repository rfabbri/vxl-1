function [] = test_david_sil21()
%TEST
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: Sep, 2006

% debug switch
DEBUG = 1;
%% parameters controlling the output
    resultpath = '../result/david_sil21/';
    fname_txt_pc = 'pointcloud.txt';
    fname_pwn_pc = 'pointcloud.pwn';
    fname_vrml_pc = 'pointcloud.wrl';
    fname_pwn_pcni = 'pointcloudni.pwn';
    fname_vrml_pcni = 'pointcloudni.wrl';
    fname_pwn_pcrs = 'pointcloudrs.pwn';
    fname_vrml_pcrs = 'pointcloudrs.wrl';
    fname_pwn_pcnirs = 'pointcloudnirs.pwn';
    fname_vrml_pcnirs = 'pointcloudnirs.wrl';

%% load the data
fprintf(1, 'Loading data and computing the edges with gradient and orientation ...\n');
    data.imnum = 21; 
    interval = round(670/data.imnum);
    series = 0:interval:669;
    datapath = '../data/david/edgemaps/';
    for i = 1:data.imnum
        fprintf(1,'[%d]',i);
        data.imnamecell{i} = [datapath 'edgemap_' num2str(series(i)) '.png'];
        im = 255*imread(data.imnamecell{i});
        [data.edgemap{i}, data.edgegradmap{i}, data.edgeormap{i}, data.edgeposmap{i}] = edgeGOPmap(im, 3, 10, 30, 10);


        if DEBUG
            imwrite(full(data.edgemap{i}), ['../debug/david_sil21/' 'edgemap_' int2strfixed(i,4) '.png']);
        end
        angle = 2*pi*series(i)/670;
        data.imMcell{i} = loadpmatrix(angle);
    end

    data.silnum = 27;
    silpath = '../data/david/silhouettes/';
    for i = 1:data.silnum
        data.silnamecell{i} = [silpath num2str((i-1)*25) '-segmented.png'];
        angle = 2*pi*(i-1)*25/670;
        data.silMcell{i} = loadpmatrix(angle);
    end

    if DEBUG
        save('../debug/david_sil21/programdata.mat');
    end

%% main work
    [pts3drawcell, normalrawcell confrawcell] = struct_from_sil(data,1,10,'david_sil21');

    if DEBUG
        save('../debug/david_sil21/programdata.mat');
    end

    %% detect and remove outliers
    maskcell = detectoutlier(pts3drawcell, confrawcell, data);
    [pts3dcell, normalcell] = removeoutlier(pts3drawcell, normalrawcell, maskcell);
    pts3d = cell2matrix(pts3dcell);
    normal = cell2matrix(normalcell);

%% extract noninterpolation part
    pts3dcellni = cell(data.imnum);
    normalcellni = cell(data.imnum);
    for i = 1:data.imnum
        pts3dcellni{i} = pts3dcell{i}(:,:,1);
        normalcellni{i} = normalcell{i}(:,:,1);
    end
    pts3dni = cell2matrix(pts3dcellni);
    normalni = cell2matrix(normalcellni);

    if DEBUG
        save('../debug/david_sil21/programdata.mat');
    end

%% save the results
    fid = fopen([resultpath fname_txt_pc], 'w+');
    pctextwrite(fid, pts3d/10);
    fclose(fid);

    fid = fopen([resultpath fname_pwn_pc], 'w+');
    pcpwnwrite(fid, pts3d/10, normal);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pc], 'w+');
    pcvrmlwrite(fid, pts3d/10);
    fclose(fid);

    fid = fopen([resultpath fname_pwn_pcni], 'w+');
    pcpwnwrite(fid, pts3dni/10, normalni);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pcni], 'w+');
    pcvrmlwrite(fid, pts3dni/10);
    fclose(fid);

    %% resample
    pts3dcellrs = cellresample(pts3dcell,5);
    normalcellrs = cellresample(normalcell,5);
    pts3drs = cell2matrix(pts3dcellrs);
    normalrs = cell2matrix(normalcellrs);

    pts3dcellnirs = cellresample(pts3dcellni,5);
    normalcellnirs = cellresample(normalcellni,5);
    pts3dnirs = cell2matrix(pts3dcellnirs);
    normalnirs = cell2matrix(normalcellnirs);
    
    fid = fopen([resultpath fname_pwn_pcrs], 'w+');
    pcpwnwrite(fid, pts3drs/10, normalrs);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pcrs], 'w+');
    pcvrmlwrite(fid, pts3drs/10);
    fclose(fid);

    fid = fopen([resultpath fname_pwn_pcnirs], 'w+');
    pcpwnwrite(fid, pts3dnirs/10, normalnirs);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pcnirs], 'w+');
    pcvrmlwrite(fid, pts3dnirs/10);
    fclose(fid);

function P = loadpmatrix(phi)
% functionality: load matrix calibration matrix after the rotation
% input:
%       - phi: rotation angle around y axis
% output:
%       - P: calibration matrix

% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: March 28, 2006

load('../data/david/Calib_Results.mat');
load('../data/david/base_extrinsics_rect.mat');

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
