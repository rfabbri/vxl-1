% This is draw_watershed_watercourse.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 19, 2012

% maxwell's definition: detect saddles, find min,max around a saddle. follow the slope line.

function assignment = draw_watershed_watercourse(I, sigma)
	[nrows,ncols] = size(I);
	[Gx, Gy] = gaussian_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	[S, mins, maxs, saddles] = get_min_max_saddle(I, sigma);
	spoints = [mins;maxs;saddles];
	assignment = zeros(size(saddles,1), size(spoints,1));
	exhaustiveobj = ExhaustiveSearcher(spoints);
	types = [ones(size(mins,1), 1);2*ones(size(maxs,1), 1);3*ones(size(saddles,1), 1)];
	inc = 2*pi/10;
	theta = (0:inc:2*pi+inc)';
	num_angles = length(theta);
	A = [cos(theta) sin(theta)];
	figure;subplot(1,2,1)	
	imagesc(S); colormap gray; axis image; axis off
	boundary_points = cell(size(saddles,1),1);
	for i = 1:size(saddles, 1)
		p = saddles(i,:);		
		B = repmat(p, num_angles, 1) + A;
		V = interp2(S, B(:,1), B(:,2), '*cubic');
		[pks,maxpos] = findpeaks(V);
		[pks,minpos] = findpeaks(-V);		
		for j = 1:length(maxpos)
			maxseed = B(maxpos(j),:);
			[idx, T, q] = slope_line(Ix, Iy, maxseed, .6, nrows, ncols, exhaustiveobj, types, 'r');
			if T ~= 0
				if T ~= 4
					assignment(i, idx) = T;
				else
					boundary_points{i} = [boundary_points{i};q];
				end
			end
		end
		for j = 1:length(minpos)
			minseed = B(minpos(j),:);
			[idx, T, q] = slope_line(Ix, Iy, minseed, -.6, nrows, ncols, exhaustiveobj, types, 'g');
			if T ~= 0
				if T ~= 4
					assignment(i, idx) = T;
				else
					boundary_points{i} = [boundary_points{i};q];
				end
			end
		end		
	end
	title('Watercourses and watersheds (Maxwell)')
	subplot(1,2,2);imagesc(S); colormap gray; axis image; axis off
	colors = 'grby';
	hold on;
	for i = 1:size(assignment,1)
		for j = 1:size(assignment,2)
			if assignment(i,j) > 0
				line([saddles(i,1) spoints(j,1)], [saddles(i,2) spoints(j,2)],'Color',colors(assignment(i,j)), 'LineWidth', 2);	
				plot([saddles(i,1) spoints(j,1)], [saddles(i,2) spoints(j,2)], 'mx', 'MarkerSize', 10, 'LineWidth', 3)
			end
		end
		bp = boundary_points{i};
		for j = 1:size(bp,1)
			line([saddles(i,1) bp(j,1)], [saddles(i,2) bp(j,2)],'Color',colors(4), 'LineWidth', 2);	
			plot([bp(j,1)], [bp(j,2)], 'cs', 'MarkerSize', 10, 'LineWidth', 3)
		end
	end
	hold off
	title('Surface Network')
end

function [idx, T,q] = slope_line(Ix, Iy, q, s, nrows, ncols,exhaustiveobj, types, c)
	hold on		
	p = q;
	pFx = 0;
	pFy = 0;
	%iter = 0;
	while true
		%iter = iter + 1;
		plot(q(1),q(2),'.', 'Color', c);
		%drawnow
		Fx = interp2(Ix, q(1), q(2));	
		Fy = interp2(Iy, q(1), q(2));
		magF = sqrt(Fx.^2+Fy.^2);
		if magF == 0
			T = 0;
			idx = -1;
			break
		end
		Fx = Fx ./ magF;
		Fy = Fy ./ magF;			
		q = q + s*[Fx Fy];
		if abs(1 + pFx*Fx + pFy*Fy) < 0.000001 
			T = 4;
			idx = -1;
			break			
		end		
		if q(1) < 1 || q(2) < 1 || q(1) > ncols || q(2) > nrows 
			T = 4;
			idx = -1;
			q(1) = max(1,min(ncols, q(1)));
			q(2) = max(1,min(nrows, q(2)));
			break
		end
		[idx, D] = knnsearch(exhaustiveobj, q);
		if D < 1 %&& iter > 10
			T = types(idx);
			break;
		end
		pFx = Fx;
		pFy = Fy;
	end
	hold off
end
