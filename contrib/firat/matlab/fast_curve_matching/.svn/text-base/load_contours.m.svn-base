% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/load_contours.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 7, 2011

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
