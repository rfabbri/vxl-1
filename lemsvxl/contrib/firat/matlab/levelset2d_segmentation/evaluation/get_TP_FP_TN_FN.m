% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/evaluation/get_TP_FP_TN_FN.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 7, 2011

function [TP, FP, TN, FN] = get_TP_FP_TN_FN(seg_folder, gt_folder, s_index, e_index)
	num_slices = e_index - s_index + 1;
	first_time = true;
	%load seg	
	for i = s_index:e_index
		Seg = load([seg_folder '/' num2str(i) '.mat']);
		if first_time
			first_time = false;
			[nrows,ncols] = size(Seg.BW);
			segs = zeros(nrows,ncols, num_slices);
			gts = zeros(nrows,ncols, num_slices);
		end
		GT = load([gt_folder '/' num2str(i) '.mat']);
		segs(:,:,i-s_index+1) = Seg.BW;
		gts(:,:,i-s_index+1) = GT.BW;
	end
	 
	intersection = gts & segs;
	TP = sum(intersection(:));
	difference = gts - segs;
	neg = difference == -1;
	pos = difference == 1;
	FP = sum(neg(:));
	FN = sum(pos(:));
	intersection2 = (~gts) & (~segs);
	TN = sum(intersection2(:));	
end


