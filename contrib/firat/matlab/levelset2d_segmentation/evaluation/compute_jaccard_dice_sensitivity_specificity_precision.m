% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/evaluation/compute_jaccard_dice_sensitivity_specificity_precision.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 7, 2011

function [J,D,SN,SP,P] = compute_jaccard_dice_sensitivity_specificity_precision(TP, FP, TN, FN)
	J = TP/(FP + TP + FN);
	D = 2*TP/(FP + 2*TP + FN);
	SN = TP/(TP+FN);
	SP = TN/(TN+FP);
	P  = TP/(TP+FP); 
end
