% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/remove_inner_contours.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 20, 2011

function contours = remove_inner_contours(contours)
    num = length(contours);
    tbd = zeros(num,1);
    k = 1;
    for i = 1:num-1
        for j = i+1:num
           if inpolygon(contours{j}(1,1), contours{j}(1,2), contours{i}(:,1), contours{i}(:,2))
                tbd(k) = j;
                k = k + 1;     
           elseif inpolygon(contours{i}(1,1), contours{i}(1,2), contours{j}(:,1), contours{j}(:,2))             
                tbd(k) = i;
                k = k + 1;
           end 
        end
    end
    tbd(k:end) = [];
    contours(tbd) = [];
end
