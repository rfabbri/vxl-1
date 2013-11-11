% This is /lemsvxl/contrib/firat/matlab/contour_related/load_contours.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 20, 2011

function contours = load_contours(filename)
    [x,y] = textread(filename,'%f%f', 'delimiter',' ');
    contours = {};
    while length(x) > 0
        l = x(1);
        cx = x(2:l+1);
        cy = y(2:l+1);
        x(1:l+1) = [];
        y(1:l+1) = [];
        contours = [contours; [cx cy]];           
    end     
end
