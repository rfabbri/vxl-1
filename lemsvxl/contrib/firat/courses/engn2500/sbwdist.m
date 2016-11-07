% This is /lemsvxl/contrib/firat/courses/engn2500/sbwdist.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 22, 2011

function DT = sbwdist(F, C)
    DT = bwdist(C);
    DT(F == 1) = -DT(F == 1);
    %DT(DT == 0) = randn(1, length(DT(DT == 0))) * 0.000001;    
end

