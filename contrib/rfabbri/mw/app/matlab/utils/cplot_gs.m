% plots similarity graphs with little images in 2D 
% For now: no zoom allowed

% INPUTS
% Pairwise similarity matrix W

load Banco_Imagens.mat
img_db = matriz_banco;

load Wfaces_diam100.mat;

% position the points manually so that clusters are close together
pts = [...
0 0
10 10
0 17
-10 -10
0 -17
-10 10
-17 0 
10 -10 
17 0]

pts1 = pts + ones(9,1)*[10 90]

pts2 = pts + ones(9,1)*[90 10]

pts = [pts ; pts1; pts2]

% call cplota
%figure
%clf
%h = cplot2(pts,'.b')

cplota2

% plot the small images on top
hold on
cplot2_img(pts,img_db,2)



