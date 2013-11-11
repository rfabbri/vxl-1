% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/is_clockwise.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 29, 2011

function cw = is_clockwise(P)
    max_xy = max(P);
    min_xy = min(P);
    PT = P';
    [Br, Bc] = find(PT == min_xy(2)); 
    Bottom = Bc(round(end/2));
    [Lr, Lc] = find(PT == min_xy(1)); 
    Left = Lc(round(end/2));
    [Tr, Tc] = find(PT == max_xy(2)); 
    Top = Tc(round(end/2));
    [Rr, Rc] = find(PT == max_xy(1)); 
    Right = Rc(round(end/2));   
    if Bottom == Left 
        C = sign(Top-Left)+sign(Right-Top)+sign(Bottom-Right);        
    else
        C = sign(Left-Bottom)+sign(Top-Left)+sign(Right-Top);
    end
    if C > 0
        cw = 1;
    elseif C < 0
        cw = 0;
    else
        error('C = 0')
    end   
end

