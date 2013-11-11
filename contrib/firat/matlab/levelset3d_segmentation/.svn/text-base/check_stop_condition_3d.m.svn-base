% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/check_stop_condition.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 21, 2011

function flag = check_stop_condition_3d(phi0, phi1)
	flag = max(max(max((phi0 - phi1).^2))) < 0.05; %0.1
end
