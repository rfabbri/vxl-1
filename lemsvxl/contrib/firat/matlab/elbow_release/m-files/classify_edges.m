% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/classify_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011

function [edg_BG, edg_FG] = classify_edges(I, edg, BG)
	[nrows, ncols] = size(I);
    %bw = imfill(I<BG,'holes');
    %bw = ~imfill(~bw,'holes');
    bw = I<BG;
    se = strel('square',11);
    bw = imdilate(bw, se);   
    L = interp2(bw, edg(:,1), edg(:,2),'*linear');  
    r = find(L==1); 
	edg_BG = edg(r,:);
	edg_FG = edg;
	edg_FG(r,:) = [];
end
