% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_one_d_subpixel_object.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 3, 2011

clear

x1 = 5;
x2 = 7;
%x1 = 5;
%x2 = 7;
len = 15;
phi = zeros(1,len);

for i = 0:len-1
	if(i < x1)
		phi(i+1) = x1 - i;
	elseif(i < x2)
		d1 = i - x1;
		d2 = x2 - i;
		phi(i+1) = -min(d1,d2);
	else
		phi(i+1) = i - x2;
	end
end
beta0 = 1;

figure;
for a = 1:6
	subplot(2,3,a);
	plot(1:len-2, phi(2:len-1))
	sgn = sign(beta0);
	phi_x_minus = sgn * (phi - circshift(phi, [0 1]));
	phi_x_plus = sgn * (circshift(phi, [0 -1]) - phi);	
	phi_x_minus(phi_x_minus > 0) = 0;
	phi_x_plus(phi_x_plus < 0) = 0;
	R = sqrt(phi_x_minus.^2 + phi_x_plus.^2);
	hold on; plot(1:len-2, sgn*R(2:end-1), 'r-'); axis([-Inf Inf -4 8])
	grid on
	set(gca,'XTick',[-100:1:100])
	set(gca,'YTick',[-100:1:100])
	phi = phi + beta0*R;
end

legend('phi','reaction flux')


