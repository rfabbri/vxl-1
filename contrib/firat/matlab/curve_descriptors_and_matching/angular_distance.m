% This is /lemsvxl/contrib/firat/matlab/curve_descriptors/angular_distance.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 29, 2011

function D = angular_distance(a, b)
    D = acos(cos(a-b));
end
