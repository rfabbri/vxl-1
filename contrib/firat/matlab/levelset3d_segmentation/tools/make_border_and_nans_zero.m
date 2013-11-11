% This is /lemsvxl/contrib/firat/matlab/levelset/make_border_and_nans_zero.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 20, 2011

function D = make_border_and_nans_zero(D)
	D(1,:,:) = 0;
	D(end,:,:) = 0;
	D(:,1,:) = 0;
	D(:, end,:) = 0;
	D(:,:,1) = 0;
	D(:,:,end) = 0;
	D(isnan(D)) = 0;	
end
