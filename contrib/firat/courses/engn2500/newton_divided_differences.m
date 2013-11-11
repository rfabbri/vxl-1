% This is /lemsvxl/contrib/firat/courses/engn2500/newton_divided_differences.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 25, 2011

function y = newton_divided_differences(f, x)
    if length(x) == 1
        y = f(x);
    else
        y = (newton_divided_differences(f, x(2:end)) - newton_divided_differences(f, x(1:end-1)))/(x(end)-x(1));
    end
end
