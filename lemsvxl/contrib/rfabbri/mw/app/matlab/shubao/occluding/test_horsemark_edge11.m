function [] = test_horsemark_edge11()
%TEST
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: Sep, 2006

% debug switch
DEBUG = 1;
%% parameters controlling the output
    resultpath = '../result/horsemark_edge11/';
    %fname_txt_pc = 'pointcloud.txt';
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
    data.imnum = 11; 
    datapath = '../data/horsemark/images/';
    anglearray = -60:12:60;
    for i = 1:data.imnum
        fprintf(1,'[%d]',i);
        if anglearray(i) < 0
            data.imnamecell{i} = [datapath 'm' num2str(abs(anglearray(i))) '.jpg'];
        elseif anglearray(i) == 0
            data.imnamecell{i} = [datapath 'p00.jpg'];
        else
            data.imnamecell{i} = [datapath 'p' num2str(abs(anglearray(i))) '.jpg'];
        end
        im = rgb2gray(imread(data.imnamecell{i}));
        [data.edgemap{i}, data.edgegradmap{i}, data.edgeormap{i}, data.edgeposmap{i}] = edgeGOPmap(im, 3, 0, 20, 11);

        if DEBUG
            imwrite(full(data.edgemap{i}), ['../debug/horsemark_edge11/' 'edgemap_' int2strfixed(i,4) '.png']);
        end
        angle = anglearray(i)/180*pi;
        data.imMcell{i} = loadpmatrix(angle);
    end

    data.silnum = 11;
    silpath = '../data/horsemark/silhouettes/';
    for i = 1:data.silnum
        if anglearray(i) < 0
            data.silnamecell{i} = [silpath 'm' num2str(abs(anglearray(i))) '.png'];
        elseif anglearray(i) == 0
            data.silnamecell{i} = [silpath 'p00.png'];
        else
            data.silnamecell{i} = [silpath 'p' num2str(abs(anglearray(i))) '.png'];
        end
        angle = anglearray(i)/180*pi;
        data.silMcell{i} = loadpmatrix(angle);
    end

    if DEBUG
        save('../debug/horsemark_edge11/programdata.mat');
    end

%% main work
    [pts3drawcell, normalrawcell, confrawcell] = struct_from_sil2(data,1,1,'horsemark_edge11');

    if DEBUG
        save('../debug/horsemark_edge11/programdata.mat');
    end

%% detect and remove outliers
    maskcell = detectoutlier(pts3drawcell, confrawcell, data);
    [pts3dcell, normalcell] = removeoutlier(pts3drawcell, normalrawcell, maskcell);
    pts3d = cell2matrix(pts3dcell);
    normal = cell2matrix(normalcell);

%% extract noninterpolation part
    pts3dcellni = cell(data.imnum-2);
    normalcellni = cell(data.imnum-2);
    for i = 1:data.imnum-2
        pts3dcellni{i} = pts3dcell{i}(:,:,1);
        normalcellni{i} = normalcell{i}(:,:,1);
    end
    pts3dni = cell2matrix(pts3dcellni);
    normalni = cell2matrix(normalcellni);

    if DEBUG
        save('../debug/horsemark_edge11/programdata.mat');
    end

%% save the results
    % fid = fopen([resultpath fname_txt_pc], 'w+');
    % pctextwrite(fid, pts3d);
    % fclose(fid);

    fid = fopen([resultpath fname_pwn_pc], 'w+');
    pcpwnwrite(fid, pts3d, normal);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pc], 'w+');
    pcvrmlwrite(fid, pts3d);
    fclose(fid);

    fid = fopen([resultpath fname_pwn_pcni], 'w+');
    pcpwnwrite(fid, pts3dni, normalni);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pcni], 'w+');
    pcvrmlwrite(fid, pts3dni);
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
    pcpwnwrite(fid, pts3drs, normalrs);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pcrs], 'w+');
    pcvrmlwrite(fid, pts3drs);
    fclose(fid);

    fid = fopen([resultpath fname_pwn_pcnirs], 'w+');
    pcpwnwrite(fid, pts3dnirs, normalnirs);
    fclose(fid);

    fid = fopen([resultpath fname_vrml_pcnirs], 'w+');
    pcvrmlwrite(fid, pts3dnirs);
    fclose(fid);

function P = loadpmatrix(phi)

fc = 250*25;
cc = [400 300];
Mint = [fc 0 cc(1);
    0 fc cc(2);
    0 0 1];

Ry = [cos(phi) 0 -sin(phi);
        0 1 0;
        sin(phi) 0 cos(phi)];   
Rckk = [1 0 0;
        0 -1 0;
        0 0 -1];
Tckk = [0 0 -100]';
R = Rckk*Ry;
Mext = [R Tckk];

P = Mint*Mext;
