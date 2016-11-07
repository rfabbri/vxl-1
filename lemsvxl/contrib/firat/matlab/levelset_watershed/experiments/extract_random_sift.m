% This is extract_random_sift.m.

% Created on: Apr 17, 2013
%     Author: firat

function extract_random_sift(data_path, img_ext, outfolder, Num)
    files = dir([data_path '/*' img_ext]);
	if ~exist(outfolder,'dir')
		mkdir(outfolder);
    end
    sigma = 2;
	for i = 1:length(files)
        I = single(rgb2gray(imread([data_path '/' files(i).name])));
        [H,W] = size(I);
        if ~exist('Num', 'var')
            tmp = vl_sift(I);
            N = size(tmp, 2);
        else
            N = Num;
        end
        rf = zeros(4, N);
        rf(1,:) = rand(1, N)*W + 1;
        rf(2,:) = rand(1, N)*H + 1;
        rf(3,:) = ones(1, N)*sigma;  
        [rand_f, rand_d] = vl_sift(I, 'frames', rf, 'orientations');
        save([outfolder '/' files(i).name '.mat'], 'rand_f', 'rand_d');
    end
end

