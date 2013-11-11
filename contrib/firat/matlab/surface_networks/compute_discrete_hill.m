% This is compute_discrete_hill.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 30, 2012

function hill = compute_discrete_hill(I, mins, maxs, saddles)
	maxp = maxs(1, [2 1]);
	[nrows,ncols] = size(I);
	se = strel('square',3);
	Q = java.util.LinkedList;
	Q.add(maxp);
	visited = zeros(size(I));
	first_time = true;
	while true
		old_visited = visited;
		%Q.size
		while Q.size > 0			
			elem = Q.remove;
			r = elem(1);
			c = elem(2);		
			P = I(r-1:r+1,c-1:c+1);
			P = P(5) - P;
			%P([1 3 7 9]) = -Inf;
			M = P >= 0; 
			[r1,c1] = find(M);
			points = [r+r1-2 c+c1-2];
			for i = 1:size(points,1)
				r2 = points(i,1);
				c2 = points(i,2);
				if r2 >1 && r2 < nrows && c2 >1 && c2 < ncols
					visited(r2,c2) = 1;
				end						
			end		
		end
		if all(all(visited == old_visited))
			break
		end
		%clf
		%imagesc(visited == 1); colormap gray; axis image
		%drawnow		
		bw2 = imerode(visited, se);	 
		front = visited - bw2;
		[r,c] = find(front);
		for i = 1:length(r)
			Q.add([r(i) c(i)]);
		end
	end
	hill = visited;
	figure;
	imagesc(I); colormap gray; axis image; axis off
	hold on;
	contour(hill, [.5,.5],'r')
	hold off
	title('Flood-Fill')
	%region_highlighter(I, .01, hill);
	%hold on;	
	%plot(mins(:,1), mins(:,2), 'rx','MarkerSize', 6,'LineWidth', 2);
	%plot(maxs(:,1), maxs(:,2), 'gx','MarkerSize', 6,'LineWidth', 2);
	%plot(saddles(:,1), saddles(:,2), 'bo','MarkerSize', 6,'LineWidth', 2);
	%plot(maxp(2), maxp(1), 'gd','MarkerSize', 6,'LineWidth', 2)
	%hold off;
end
