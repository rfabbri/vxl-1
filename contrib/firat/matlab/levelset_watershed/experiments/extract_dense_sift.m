% This is extract_dense_sift.m.

% Created on: Apr 22, 2013
%     Author: firat
% n (height) x m (width)

function extract_dense_sift(data_path, img_ext, outfolder, n, m)
    files = dir([data_path '/*' img_ext]);
	if ~exist(outfolder,'dir')
		mkdir(outfolder);
    end
    sigma = 2;
    N = n*m;
	for i = 1:length(files)
        I = single(rgb2gray(imread([data_path '/' files(i).name])));
        [H,W] = size(I);
        h = H/(n+1);
        w = W/(m+1);
        [x,y] = meshgrid(w:w:w*m, h:h:h*n);        
        rf = zeros(4, N);
        rf(1,:) = x(:)';
        rf(2,:) = y(:)';
        rf(3,:) = ones(1, N)*sigma;  
        [dense_f, dense_d] = vl_sift(I, 'frames', rf, 'orientations');
        save([outfolder '/' files(i).name '.mat'], 'dense_f', 'dense_d');
    end    
end

