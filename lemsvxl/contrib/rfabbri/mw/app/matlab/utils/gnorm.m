function [xnew]=gnorm(x,xsm)

% Normalizes the smoothed results yielded by 
% gsm (1d) as to "prevent" gaussian shrinkage.

tam=max(size(x));
Mx=max(x); mx=min(x);
Mxsm=max(xsm); mxsm=min(xsm);

xnew= ( ( (xsm - mxsm).*(Mx-mx) )./(Mxsm-mxsm) ) + mx;

