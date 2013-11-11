% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/reinitlevelset_narrow.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 17, 2011

function phi = reinitlevelset_narrow(phi, t)
	phi(phi < -t) = -t;
	phi(phi > t) = t;
end
