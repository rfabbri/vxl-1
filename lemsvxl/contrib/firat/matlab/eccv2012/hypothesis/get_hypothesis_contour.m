% This is get_hypothesis_contour.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 19, 2012

function [hypo_x, hypo_y] = get_hypothesis_contour(model_contour, hypo)
    %sc = hypo(4)*cos(hypo(3));
    %ss = hypo(4)*sin(hypo(3));
    %T = [ sc -ss;
    %      ss  sc;
    %      hypo(1)  hypo(2)];  
    %t = maketform('affine',T);
    [hypo_x, hypo_y] = tformfwd(hypo, model_contour(:,1), model_contour(:,2));    
end
