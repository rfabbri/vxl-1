% This is bad_repeatability_illumination_patch_analysis.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 11, 2012

function bad_repeatability_illumination_patch_analysis(infolder, sigma, critical_point_type)
	files = dir(fullfile(infolder, '*.png'));
	K = length(files)/2;
	for i = 1:K
		I = double(rgb2gray(imread(fullfile(infolder, ['I' num2str(i) '.png']))));
		J = double(rgb2gray(imread(fullfile(infolder, ['J' num2str(i) '.png']))));
		[S, mins1, maxs1, saddles1] = get_critical_points_with_scores(I, sigma);
		[S, mins2, maxs2, saddles2] = get_critical_points_with_scores(J, sigma);
		if critical_point_type == 1
			L1 = mins1;
			L2 = mins2;
		elseif critical_point_type == 2
			L1 = maxs1;
			L2 = maxs2;
		else
			L1 = saddles1;
			L2 = saddles2;
		end
		bad_repeatability_illumination(I, J, L1, L2, sqrt(2));
		pause
		close all
	end
end
