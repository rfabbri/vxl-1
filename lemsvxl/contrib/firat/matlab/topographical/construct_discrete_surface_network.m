% This is construct_discrete_surface_network.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Oct 1, 2012

function construct_discrete_surface_network(I, sigma, mode)
	ind = [1 2 3 6 9 8 7 4 1 2];
	if ~exist('mode','var')
		mode = 'noscore';
	end
	if strcmp(mode,'score')
		[S, mins, maxs, saddles] = get_critical_points_with_scores(I, sigma);
	else
		[S, mins, maxs, saddles] = get_critical_points(I, sigma);		
	end
	I = S;
	I = [I(2,:); I; I(end-1,:)];
	I = [I(:,2), I, I(:,end-1)];
	I(1,1) = I(3,3);
	I(1, end) = I(3, end-2);
	I(end, 1) = I(end-2, 3);
	I(end, end) = I(end-2, end-2);
	[H,W] = size(I);
	CM = zeros(H,W);
	mindex = sub2ind([H,W], mins(:,2)+1, mins(:,1)+1);
	maxdex = sub2ind([H,W], maxs(:,2)+1, maxs(:,1)+1);
	saddex = sub2ind([H,W], saddles(:,2)+1, saddles(:,1)+1);
	CM(mindex) = 1;
	CM(maxdex) = 2;
	CM(saddex) = 3;
	
	links = java.util.LinkedList;
	for i = 1:length(saddex)
		x = saddles(i,1)+1;
		y = saddles(i,2)+1;
		P = I(y-1:y+1, x-1:x+1);		
		V = P(ind);
		[maxv, maxloc] = findpeaks(V);
		for i = 1:length(maxloc)
			ind0 = ind(maxloc(i));
			[dy, dx] = ind2sub([3,3], ind0);
			x0 = x - 2 + dx;
			y0 = y - 2 + dy; 
			sl = java.util.LinkedList;
			sl.add([x-1 y-1 3]);
			CM = trace_slope_line(I, H, W, ind, CM, sl, x0, y0, 1);
			links.add(sl);
		end
		[minv, minloc] = findpeaks(-V);
		for i = 1:length(minloc)
			ind0 = ind(minloc(i));
			[dy, dx] = ind2sub([3,3], ind0);
			x0 = x - 2 + dx;
			y0 = y - 2 + dy; 
			sl = java.util.LinkedList;
			sl.add([x-1 y-1 3]);
			CM = trace_slope_line(I, H, W, ind, CM, sl, x0, y0, -1);
			links.add(sl);
		end
		
	end
	%CM = CM(2:end-1, 2:end-1);
	figure; imagesc(I(2:end-1, 2:end-1)); axis image; axis off; colormap gray
	hold on
	colors = 'gry';
	shapes = 'v^o';
	for i = 1:links.size
		sl = links.pop;
		if sl.size == 1
			continue
		end
		sl = reshape(cell2mat(cell(sl.toArray(sl))), 3, sl.size())';	
		if sl(end,3) >= 4
			continue
		end			
		plot(sl(1:end,1), sl(1:end,2), [colors(sl(end,3)) '-'], 'LineWidth', 2);			
		plot(sl(1,1), sl(1,2),['b' shapes(sl(1,3))], 'MarkerSize', 8, 'LineWidth', 2);
		plot(sl(end,1), sl(end,2),['b' shapes(sl(end,3))], 'MarkerSize', 8, 'LineWidth', 2);		
		
	end
	
	
end

function CM = trace_slope_line(I, H, W, ind, CM, sl, x0, y0, direc)	
	if CM(y0,x0) > 0 && CM(y0,x0) < 4
		sl.add([x0-1, y0-1, CM(y0,x0)]);
		return;
	end
	CM(y0,x0) = 5 + direc;		
	if x0 == 1 || y0 == 1 || x0 == W || y0 == H
		return
	end
	sl.add([x0-1, y0-1, CM(y0,x0)]);
	P = I(y0-1:y0+1, x0-1:x0+1);		
	V = P(ind);
	[maxval, maxind] = max(direc*V);
	ind0 = ind(maxind);
	[dy, dx] = ind2sub([3,3], ind0);
	x1 = x0 - 2 + dx;
	y1 = y0 - 2 + dy;
	CM = trace_slope_line(I, H, W, ind, CM, sl, x1, y1, direc);
end

