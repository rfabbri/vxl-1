% This is sbwdist.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function DT = sbwdist(F, hx, hy, hz)
	[nrows,ncols,nbands] = size(F);
	[gridx, gridy, gridz] = meshgrid(1:hx:ncols, 1:hy:nrows, 1:hz:nbands);
    bw = interp3(F, gridx, gridy, gridz) >= 1;     
    se = strel('square',3);
	bw2 = imerode(bw, se);	 
	bw3 = bw - bw2;  	 
	[ar,ac] = find(bw3);	
	DT = subpixelDT_fast([(ac-1)*hx+1 (ar-1)*hy+1], nrows, ncols, hx, hy);	
	DT(bw) = -DT(bw);      
end

