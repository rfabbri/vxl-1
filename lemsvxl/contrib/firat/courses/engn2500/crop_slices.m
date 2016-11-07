% This is /lemsvxl/contrib/firat/courses/engn2500/crop_slices.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 12, 2011

function W = crop_slices(V, bb)
    [rV, cV, bV] = size(V);
    bb
    W = zeros(bb(4)+1, bb(3)+1, bV);
    for i = 1:bV
        W(:,:,i) = imcrop(V(:,:,i), bb);        
    end
end

