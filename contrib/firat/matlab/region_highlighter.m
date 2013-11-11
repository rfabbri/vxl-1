% This is /lemsvxl/contrib/firat/matlab/region_highlighter.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Dec 11, 2011

function BW = region_highlighter(I, alpha, BW)
    if ~exist('BW','var')
    	BW = roipoly(I);
    end
    %close all;
    M = (~BW)*alpha + BW;
    if I == 0
    	h = imhandles(gcf);
    	
    elseif isfloat(I)
    	h = imagesc(I); colormap gray; axis image; axis off
    else
    	h = imshow(I);
    end
    set(h, 'AlphaData', M);
end
