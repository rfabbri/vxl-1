% This is /lemsvxl/contrib/firat/matlab/levelset2d/upwinding2d.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 3, 2011


function R = upwinding2d(phi, beta0)
	
	sgn = sign(beta0);
	a = (phi - circshift(phi, [0 1]));
	b =  (circshift(phi, [0 -1]) - phi);	
	c =  (phi - circshift(phi, [1 0]));
	d =  (circshift(phi, [-1 0]) - phi);
	
	aplus = a;
	aplus(a < 0) = 0;
	aminus = -a;
	aminus(a > 0) = 0;
	bplus = b;
	bplus(b < 0) = 0;
	bminus = -b;
	bminus(b > 0) = 0;
	cplus = c;
	cplus(c < 0) = 0;
	cminus = -c;
	cminus(c > 0) = 0;
	dplus = d;
	dplus(d < 0) = 0;
	dminus = -d;
	dminus(d > 0) = 0;		
	
	R = sqrt(max(aminus.^2, bplus.^2) + max(cminus.^2, dplus.^2) + max(aplus.^2, bminus.^2) + max(cplus.^2, dminus.^2))/sqrt(2);
	
	
	R(1,:) = R(2,:);
	R(end,:) = R(end-1,:);
	R(:,1) = R(:,2);
	R(:, end) = R(:, end-1);	
	
	%imagesc(R);colormap gray; axis image;pause
end
