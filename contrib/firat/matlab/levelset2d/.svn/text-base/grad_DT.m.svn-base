% This is /lemsvxl/contrib/firat/matlab/levelset2d/grad_DT.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 7, 2011

function [DTx, DTy] = grad_DT(DT, points, idx, hx, hy)
	[M,N] = size(DT);
    maxx = (N-1)*hx + 1;
    maxy = (M-1)*hy + 1;
	X = repmat(1:hx:maxx,M,1);
	Y = repmat((1:hy:maxy)',1,N);	 
	idx = idx';
	Xk = vec2mat(points(idx(:),1), N);
	Yk = vec2mat(points(idx(:),2), N);	
	DTx = (X - Xk)./DT;
	DTy = (Y - Yk)./DT;
	DTx(DT == 0) = 0;
	DTy(DT == 0) = 0;
end
