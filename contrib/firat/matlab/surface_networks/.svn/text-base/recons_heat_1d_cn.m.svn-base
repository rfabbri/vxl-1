% This is recons_heat_1d_cn.m
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 14, 2012

function recons_heat_1d_cn(x, y, index, value)
	len = length(x);
	CNleft = eye(len);
	CNright = eye(len);
	L = 1;
	z = (max(value)-min(value)+5)*rand(len, 1)+min(value);
	z(1) = y(1);
	z(end) = y(end);
	z(index) = value;
	for i = 2:len-1
		CNleft(i, i-1:i+1) = [-L  (1+2*L)  -L];	
		CNright(i, i-1:i+1) = [L  (1-2*L)  L];	
	end
	for i = 1:length(index)
		j = index(i);
		CNleft(j,j-2:j+2) =[0 0 1 0 0];
		CNright(j,j-2:j+2) =[0 0 1 0 0];
	end
	M = inv(CNleft)*CNright;
	z = (M^10000)*z;
	%for i = 1:10000
	%	z = CNleft\(CNright*z);		
	%	clf;
		
		plot(x, z);
		drawnow
	%end
end
