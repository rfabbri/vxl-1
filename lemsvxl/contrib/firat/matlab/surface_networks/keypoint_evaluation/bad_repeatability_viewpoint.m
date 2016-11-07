% This is bad_repeatability_viewpoint.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 15, 2012

function bad_repeatability_viewpoint(I, J, locs1, locs2, final_correspMap, final_pMap, th)
	L1 = round(locs1);
	L2 = round(locs2);
	size1 = size(I);
	size2 = size(J);
	D = Inf(size(L1,1), size(L2,1));
	ignore_mask = zeros(size(L1,1), 1);	
	for i = 1:size(L1,1)
		ind = (L1(i,2)-1)*size1(2)+L1(i,1);
		[row,col] = ind2sub(size2, final_correspMap{ind}(final_pMap{ind} >= th));
		
		if ~isempty(row)
			D(i,find(ismember(L2, [col; row]', 'rows'))) = 0;
		else
			ignore_mask(i) = 1;
		end
	end
	ignore_mask = logical(ignore_mask);
	%ignore_id = find(ignore_mask);
	[Matching,Cost] = assignmentoptimal(D);
	bad1 = find(Matching == 0 & ignore_mask == 0);
	u = unique(Matching);
	bad2 = setdiff(1:size(L2,1), u(2:end)); 
	
	figure; imagesc(I); colormap gray; axis image; axis off
	hold on;
	plot(L1(~ignore_mask, 1), L1(~ignore_mask, 2), 'go');
	plot(L1(ignore_mask, 1), L1(ignore_mask, 2), 'co');
	plot(L1(bad1, 1), L1(bad1, 2), 'ro');
	hold off;
	title('Img 1')
	legend({'good', 'ignored', 'bad'},'Location', 'NorthEastOutside')
	
		
	figure; imagesc(J); colormap gray; axis image; axis off
	hold on;
	plot(L2(u(2:end),1), L2(u(2:end), 2), 'go');
	plot(L2(bad2,1), L2(bad2, 2), 'ro');
	hold off;
	title('Img 2')
	legend({'good', 'bad'},'Location', 'NorthEastOutside')
end
