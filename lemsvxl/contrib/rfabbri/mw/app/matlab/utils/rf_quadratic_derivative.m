% ----------------------------------
function linear_fn_as_quadratic = rf_quadratic_derivative(quadratic)
% Returns a linear function representing the derivative of the quadratic.
% The linear funciton is simply returned as a quadratic.

ax = 2*quadratic.coeffs(1);
b  = quadratic.coeffs(2);
linear_fn_as_quadratic.coeffs = [0 ax b];

