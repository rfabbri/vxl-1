% This is bad_repeatability_illumination.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 14, 2012

function bad_repeatability_illumination(I, J, L1, L2, th)
	D = pdist2(L1, L2);
	D(D > th) = Inf;
	[Matching,Cost] = assignmentoptimal(D);
	bad1 = find(Matching == 0);
	u = unique(Matching);
	bad2 = setdiff(1:size(L2,1), u(2:end));
	figure; imagesc(I); colormap gray; axis image; axis off
	hold on;
	plot(L1(:, 1), L1(:, 2), 'go');
	plot(L1(bad1, 1), L1(bad1, 2), 'ro');
		
	hold off;
	title('Img 1')
	
	figure; imagesc(J); colormap gray; axis image; axis off
	hold on;
	plot(L2(:, 1), L2(:, 2), 'go');	
	plot(L2(bad2, 1), L2(bad2, 2), 'ro');
	hold off;
	title('Img 2')
	
	
end
