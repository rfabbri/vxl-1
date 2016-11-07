% ----------------------------------
function quadratic = rf_quadratic(a, b, c)
% Interpolates quadratic to 3 points whose y coordinates are given by the input
% [a b c].
% The respective x coordinates are taken to be [-1 0 1].
% so that we're interpolating (-1, a), (0, b), (1, c).
%
% output:
%
%   quadratic.coeffs - 1x3 vector of the coefficients of x^2 x and constant, resp.

A = 0.5*(a + c - 2*b);
B = 0.5*(c - a);
D = b;
quadratic.coeffs = [A B D];
