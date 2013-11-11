% This is /lemsvxl/contrib/firat/matlab/levelset2d/experiments/test_upwinding.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 3, 2011

clear

x1 = 5.1;
x2 = 5.2;
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
beta0 = -1;
sgn = sign(beta0);

figure;
for q = 1:6
	subplot(2,3,q);
	plot(0:len-1, phi)
	a = (phi - circshift(phi, [0 1]));
	b =  (circshift(phi, [0 -1]) - phi);
	aplus = a;
	aplus(a < 0) = 0;
	aminus = -a;
	aminus(a > 0) = 0;
	bplus = b;
	bplus(b < 0) = 0;
	bminus = -b;
	bminus(b > 0) = 0;
	
	
	R = sqrt(max(aminus.^2, bplus.^2) + max(aplus.^2, bminus.^2))/sqrt(2) ;
		
	R(1) = phi(1) - phi(2);
	R(end) = phi(end) - phi(end-1);
	
	
		
	hold on; plot(0:len-1, sgn*R, 'r-'); axis([-Inf Inf -4 8])
	%plot(1:len-2, sgn*R2(2:end-1), 'g-'); axis([-Inf Inf -4 8])
	hold off
	grid on
	set(gca,'XTick',[-100:1:100])
	set(gca,'YTick',[-100:1:100])
	
	phi = phi + beta0*R;
end

legend('phi','reaction flux')


