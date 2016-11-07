% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_upwinding2.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 4, 2011

clear

%x1 = 5.1;
%x2 = 5.4;
x1 = 3;
x2 = 9;
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
beta0 = -1;
sgn = sign(beta0);
figure;
for q = 1:6
	subplot(2,3,q);
	plot(0:len-1, phi)
	phi_x_minus =  (phi - circshift(phi, [0 1]));
	phi_x_plus =  (circshift(phi, [0 -1]) - phi);	
	
	if beta0 > 0	
		min_phi_x_minus = phi_x_minus;
		min_phi_x_minus(phi_x_minus > 0) = 0;
		max_phi_x_plus = phi_x_plus;
		max_phi_x_plus(phi_x_plus < 0) = 0;	
		R = sqrt(min_phi_x_minus.^2 + max_phi_x_plus.^2);
		degenerate = find(min_phi_x_minus ~=  0 & max_phi_x_plus ~= 0);
		R(degenerate) = (abs(phi_x_minus(degenerate)) + abs(phi_x_plus(degenerate)))/2;		
	else
		max_phi_x_minus = phi_x_minus;
		max_phi_x_minus(phi_x_minus < 0) = 0;
		min_phi_x_plus = phi_x_plus;
		min_phi_x_plus(phi_x_plus > 0) = 0;			
		R = sqrt(max_phi_x_minus.^2 + min_phi_x_plus.^2);
		degenerate = find(max_phi_x_minus ==  0 & min_phi_x_plus == 0);
		R(degenerate) = (abs(phi_x_minus(degenerate)) + abs(phi_x_plus(degenerate)))/2;
	end
	
	
		
	R(1) = R(2);
	R(end) = R(end-1);
		
	hold on; plot(0:len-1, sgn*R, 'r-'); axis([-Inf Inf -4 8])
	%plot(1:len-2, sgn*R2(2:end-1), 'g-'); axis([-Inf Inf -4 8])
	hold off
	grid on
	set(gca,'XTick',[-100:1:100])
	set(gca,'YTick',[-100:1:100])
	
	phi = phi + beta0*R;
end

legend('phi','reaction flux')


