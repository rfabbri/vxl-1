% This is extract_surface_network_matlab.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Oct 30, 2012

function [count,ws_count,wc_count] = count_slope_districts(I, sigma)
	[nrows,ncols] = size(I);
	if sigma > 0
		G = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		S = imfilter(I,G,'replicate');
	else
		S = I;
	end
	WS = watershed(S, 8); %watershed = max + saddle + ridges
	WC = watershed(-S, 8); %watercourse = min + saddle + valleys
	natdist = bwlabel((WS>0) & (WC>0));
	count = max(natdist(:));
	ws_count = max(WS(:));
	wc_count = max(WC(:));
	
	if 0
		clear
		dataset_path = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals';
		img_ext = '.jpg';
		files = dir([dataset_path '/*' img_ext]);
		sd_count = zeros(1,length(files));
		ws_count = zeros(1,length(files));
		wc_count = zeros(1,length(files));
		for i = 1:length(files)
			fprintf('processing %s\n',[dataset_path '/' files(i).name]);
			I = imread([dataset_path '/' files(i).name]);
			[a,b,c] = size(I);
			if c > 1
				I = rgb2gray(I);
			end
			I = double(I);
			[sd_count(i), ws_count(i), wc_count(i)] = count_slope_districts(I, 0);
		end
	
	end
	
	
	if 0 
		clear
		addpath /home/firat/Downloads/siftDemoV4
		dataset_path = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals';
		img_ext = '.jpg';
		files = dir([dataset_path '/*' img_ext]);
		count = zeros(1,length(files));
		for i = 1:length(files)
			fprintf('processing %s\n',[dataset_path '/' files(i).name]);
			[image, descriptors, locs] = sift([dataset_path '/' files(i).name]);			
			count(i) = size(descriptors,1);
		end
	end
	
	if 0 
		clear
		addpath /home/firat/Downloads/siftDemoV4
		dataset_path = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals';
		img_ext = '.jpg';
		files = dir([dataset_path '/*' img_ext]);
		count = zeros(1,length(files));
		for i = 1:length(files)
			fprintf('processing %s\n',[dataset_path '/' files(i).name]);
			I = imread([dataset_path '/' files(i).name]);
			[a,b,c] = size(I);
			if c > 1
				I = rgb2gray(I);
			end
			I = double(I); 
			corners = corner(I, 'Harris', 10^20);						
			count(i) = size(corners,1);
		end
	end
end
