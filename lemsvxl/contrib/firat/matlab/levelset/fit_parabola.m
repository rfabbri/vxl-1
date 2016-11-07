function [a,b,c] = fit_parabola(x,y)
	A = [x.^2 x ones(3,1)];
	B = inv(A)*y;
	a = B(1);
	b = B(2);
	c = B(3);
end

