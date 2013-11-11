% This is csaWrapper.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 24, 2012

function Matching = csaWrapper(D)
	resolution = 100;
	D = D*resolution;
	[H,W] = size(D);
	Large = double(intmax('int32'));
	D(isinf(D)) = Large;
	if H > W
		D = [D, Large*ones(H, H-W)];
	elseif W > H
		D = [D; Large*ones(W-H, W)];
	end
	n = max(H,W);
	edges = csaAssign(2*n,sparsify(D));
	if H >= W
		Matching = (edges(2,:) - H)';
		Matching(Matching > W) = 0;
		Matching(edges(3,:) == Large) = 0;
	else
		Matching = (edges(2, 1:H) - W)';
		Matching(Matching > H) = 0;
		Matching(edges(3,1:H) == Large) = 0;
	end	
end
