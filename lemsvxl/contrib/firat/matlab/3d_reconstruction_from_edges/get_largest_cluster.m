% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/get_largest_cluster.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 20, 2011

function A = get_largest_cluster(clusters)
    maxSize = -Inf;
    maxIndex = -1;
    for i = 1:length(clusters)
        if size(clusters{i}, 1) > maxSize
            maxSize = size(clusters{i}, 1);
            maxIndex = i;
        end
    end
    A = clusters{maxIndex};
end
