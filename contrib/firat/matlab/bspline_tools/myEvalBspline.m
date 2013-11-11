% This is myEvalBspline.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 15, 2012

function z = myEvalBspline(Bsf, x, y)
	global BsplinePad
	if prod(size(x)) == 1 % single point
		z = evalBsplineNpoint(Bsf.c, Bsf.N, [y+BsplinePad x+BsplinePad], Bsf.bsh);
	else
		z = evalBsplineNpoints(Bsf.c, Bsf.N, {y+BsplinePad,x+BsplinePad}, Bsf.bsh);
	end
end
