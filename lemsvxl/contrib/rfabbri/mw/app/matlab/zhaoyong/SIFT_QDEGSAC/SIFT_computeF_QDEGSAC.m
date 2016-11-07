clear

dThresh = 1.5;          % inlier pixel distance threshold
gcThresh = .8;          % guided match correlation threshold
gdThresh = 1.5;

[file1, pathname1] = uigetfile({'*.jpg';'*.bmp';'*.tiff';'*.png';'*.pgm'},'File Selector');
[file2, pathname2] = uigetfile({'*.jpg';'*.bmp';'*.tiff';'*.png';'*.pgm'},'File Selector');


fprintf( 'Loading images...' );
tic 
img1 = rgb2gray(imread(strcat(pathname1, file1)));
img2 = rgb2gray(imread(strcat(pathname2, file2)));
%img1 = imread(strcat(pathname1, file1));
%img2 = imread(strcat(pathname2, file2));
fprintf( '%f\n', toc );

% load the SIFT frames and features
SIFT_file1 = strcat(file1, '_SIFTs.mat');
if exist(SIFT_file1) == 0
    fprintf( 'Calculating SIFTs for image 1...' )
    tic
    
    [loc,des,gss,dogss] = sift( img1, ...
                                    'firstoctave', 0, ...
                                    'verbosity', 0, ...
                                    'Threshold', 0.003, ...
                                    'NumLevels', 5, ...
                                    'sigman', 0.5) ;
                             
    fprintf( '%f\n', toc );
    save(SIFT_file1, 'loc', 'des');
    loc_1 = loc;
    des_1 = des;
    clear 'gss';
    clear 'dogss';
    clear 'loc';
    clear 'des';
else
    load(SIFT_file1);
    loc_1 = loc;
    des_1 = des;
    clear 'loc';
    clear 'des';    
end

SIFT_file2 = strcat(file2, '_SIFTs.mat');
if exist(SIFT_file2) == 0
    fprintf( 'Calculating SIFTs for image 2...' )
    tic
    
    [loc,des,gss,dogss] = sift( img2, ...
                                    'firstoctave', 0, ...
                                    'verbosity', 0, ...
                                    'Threshold', 0.003, ...
                                    'NumLevels', 5, ...
                                    'sigman', 0.5) ;
                             
    
    fprintf( '%f\n', toc );
    save(SIFT_file2, 'loc', 'des');
    loc_2 = loc;
    des_2 = des;
    clear 'gss';
    clear 'dogss';
    clear 'loc';
    clear 'des';
    
else
    load(SIFT_file2);
    loc_2 = loc;
    des_2 = des;
    clear 'loc';
    clear 'des';
end


fprintf('Calculating SIFT matches...');
tic

sift_matches = get_sift_matches(des_1, des_2, 0.8);
fprintf( '%f seconds\n', toc );    

if sift_matches == 0
    fprintf('there is NO matched SIFT feature found!\n');
    return;
end;

if size(sift_matches, 1) < 8
    fprintf('too few SIFT feature found!\n');
    return;    
end;


m1 = loc_1( [1:2], sift_matches(:,1));
m2 = loc_2( [1:2], sift_matches(:,2));

SIFT_showfeatures;


[bF,m1in1,m2in1,m1in2,m2in2,m1in,m2in,bF1,bin,bin2,bin3,it,it2,it3,m1out2,m2out2,ttin,ttin2] = QDEGSAC(m1,m2,20.0,0.98, 0.70, ...
	9,8,'GetLinearEquationsF','GetSolutionFLinear','GetFInlierFromLinear');

F = bF;

SIFT_showmatches;
disp 'Done'    
