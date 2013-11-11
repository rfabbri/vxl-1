% This is /lemsvxl/contrib/firat/matlab/levelset/fastreinitlevelset.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 15, 2011

function I = fastreinitlevelset(C, phi, M, N)
	J = zeros(M,N);
	while ~isempty(C)
		num = C(2,1);
		points = C(:, 2:num+1);
		mask = poly2mask(points(1,:), points(2,:),M,N);
		J = J + mask;
		C(:,1:num+1) = [];
	end
	se = strel('disk',1);
	bw2 = imerode(J, se);
	bw3 = J - bw2;
	I = sbwdist(J, bw3);	
end
