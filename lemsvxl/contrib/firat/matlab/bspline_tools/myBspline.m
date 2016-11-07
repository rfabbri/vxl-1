% This is myBspline.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 15, 2012

function [Bsf,pad] = myBspline(f,n)
	global BsplinePad
	BsplinePad = 5;
	f = [repmat(f(1,:),BsplinePad,1);f;repmat(f(end,:),BsplinePad,1)];
	f = [repmat(f(:,1),1,BsplinePad),f,repmat(f(:,end),1,BsplinePad)];
	Bsf = Bspline(f,n);
end
