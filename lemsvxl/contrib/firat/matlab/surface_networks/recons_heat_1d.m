% This is recons_heat_1d.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 24, 2012

function recons_heat_1d(x, y, index, value)
	len = length(x);
	M = eye(len);
	L = .25;
	%index = find(z(2:end-1) ~= 0)+1;
	%value = z(index);
	z = (max(value)-min(value)+5)*rand(len, 1)+min(value);
	z(1) = y(1);
	z(end) = y(end);
	z(index) = value;
	
	for i = 2:len-1
		M(i, i-1:i+1) = [L  (1-2*L)  L];		
	end
	for i = 1:length(index)
		j = index(i);
		M(j-1:j+1, j-2:j+2) = [L/2 (1-2*L)/2 L (1-2*L)/2 L/2; 0 0 1 0 0; L/2 (1-2*L)/2 L (1-2*L)/2 L/2];
		%M(j,j-2:j+2) =[0 0 1 0 0];
	end
	%figure
	
	%for i = 1:10000
		z = (M^350000)*z;
		
		%clf;
		
		%plot(x, z);
		%drawnow
	%end
	figure;
	R = z;
	plot(x,R)
	hold on
	plot(x,y,'r')
	hold off
end
