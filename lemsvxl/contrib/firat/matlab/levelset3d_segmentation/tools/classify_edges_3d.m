% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/classify_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011

function [edg_BG, edg_FG] = classify_edges_3d(I, edg, BG)
	[nrows, ncols, nbands] = size(I);
    bw = I<BG;
    se = strel(ones(11,11,11));
    bw = imdilate(bw, se);   
    L = interp3(bw, edg(:,1), edg(:,2), edg(:,3), '*linear');  
    r = find(L==1); 
	edg_BG = edg(r,:);
	edg_FG = edg;
	edg_FG(r,:) = [];
end
