% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/check_stop_condition.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 21, 2011
%default value 0.05

function flag = check_stop_condition(phi0, phi1, thresh)
	flag = max(max((phi0 - phi1).^2)) < thresh; %0.1
end
