% This is /lemsvxl/contrib/firat/courses/engn2500/eno_zero_xings.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 25, 2011

function eno_zero_xings(f)
    N = length(f);
    f = [f(1), f, f(end)]    
    for j = 2:N
        xj = j;
        xjplus1 = j+1;
        A = f(xj);
        B = newton_divided_differences(f, [xj,xjplus1]);
        kmin = j;
        a2 = newton_divided_differences(f, kmin:kmin+2);
        b2 = newton_divided_differences(f, kmin-1:kmin+1);
        if abs(a2) >= abs(b2)
            c2 = b2;
        else
            c2 = a2;    
        end
        T1 = c2;
        T2 = B-xj-xjplus1;
        T3 = A-B*xj+c2*xj*xjplus1;
        R = roots([T1 T2 T3])        
    end
end



