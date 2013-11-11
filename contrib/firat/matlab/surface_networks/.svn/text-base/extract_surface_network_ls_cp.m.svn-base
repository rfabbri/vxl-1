% This is extract_surface_network2.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 20, 2012

% maxwell's definition: detect saddles, find min, max around a saddle. follow the slope line.
% Point types:
% 1 -> min
% 2 -> max
% 3 -> saddle
% 4 -> undetected

function [mins, maxs, saddles, assignments, slope_lines] = extract_surface_network_ls_cp(I, sigma)
	addpath /home/firat/lemsvxl/src/contrib/firat/matlab/levelset_watershed
	[nrows,ncols] = size(I);
	[Gx, Gy] = gaussian_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	%[S, ign, ign, saddles] = get_min_max_saddle(I, sigma);
	fprintf('Start saddle detection\n');
	[mins, maxs, saddles, S] = levelset_min_max_saddle_detector(I, sigma, false); 
	%saddles = [20.65 13.97];
	fprintf('End saddle detection\n');
	inc = 2*pi/30;
	theta = (0:inc:2*pi+inc)';
	num_angles = length(theta);
	A = 1.01*[cos(theta) sin(theta)];
	figure;subplot(1,2,1)	
	imagesc(S); colormap gray; axis image; axis off
	hold on; 
    plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);	   
    plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);   
    hold off;
	undetected = cell(size(saddles,1),1);
	%mins = [nrows+5 ncols+5];
	%maxs = mins;
	assignments = [];
	slope_lines = cell(size(saddles,1)*4, 1);
	k = 1;
	colors = [[255 160 0]/255; [139, 0, 255]/255; [1 0 0]; [0 1 0]];
	% 1 -> watercourse
	% 2 -> watershed
	for i = 1:size(saddles, 1)
		p = saddles(i,:);		
		B = repmat(p, num_angles, 1) + A;
		V = interp2(S, B(:,1), B(:,2), '*cubic');
		[pks,maxpos] = findpeaks(V);
		[pks,minpos] = findpeaks(-V);
		for j = 1:length(maxpos)
			maxseed = B(maxpos(j),:);
			[T, q, slope_lines{k}, assignments] = slope_line(Ix, Iy, i, maxseed, .2, nrows, ncols, saddles, mins, maxs, assignments, colors(2,:));			
			%[mins, maxs, assignments] = update_assignments(q, T, i, mins, maxs, saddles, assignments);
			k = k + 1;									
		end
		for j = 1:length(minpos)
			minseed = B(minpos(j),:);
			[T, q, slope_lines{k}, assignments] = slope_line(Ix, Iy, i, minseed, -.2, nrows, ncols, saddles, mins, maxs, assignments, colors(1,:));			
			%[mins, maxs, assignments] = update_assignments(q, T, i, mins, maxs, saddles, assignments);	
			k = k + 1;		
		end		
	end
		
	title('Separatrices')
	subplot(1,2,2);imagesc(S); colormap gray; axis image; axis off
	
	
	shapes = 'o^+';
	hold on;
	for i = 1:size(assignments, 1)
		T = assignments(i, 1);
		sad = saddles(assignments(i, 2), :);
		if T == 1
			p = mins(assignments(i, 3),:);
		elseif T == 2
			p = maxs(assignments(i, 3),:);
		elseif T == 3
			p = saddles(assignments(i, 3),:);
		end
		line([sad(1) p(1)], [sad(2) p(2)],'Color', colors(T,:), 'LineWidth', 2);
		plot([sad(1) ], [sad(2) ],[shapes(3)],'Color', [0, 127, 255]/255, 'MarkerSize', 10, 'LineWidth', 3);
		plot([p(1) ], [p(2) ],[shapes(T)],'Color', [0, 127, 255]/255, 'MarkerSize', 10, 'LineWidth', 3);
	end	
	hold off
	title('Surface Network')
	slope_lines = slope_lines(1:k-1);
	maxs = maxs(2:end,:);
	mins = mins(2:end,:);
end

function [mins, maxs, assignments] = update_assignments(q, T, i, mins, maxs, saddles, assignments)
	if T == 1
		[idx, D] = knnsearch(mins, q);
		if D < 1
			assignments = [assignments; T, i, idx]; 
		else
			assignments = [assignments; T, i, size(mins,1)+1];
			mins = [mins; q];
		end
	elseif T == 2
		[idx, D] = knnsearch(maxs, q);
		if D < 1
			assignments = [assignments; T, i, idx]; 
		else
			assignments = [assignments; T, i, size(maxs,1)+1];
			maxs = [maxs; q];
		end
	else
		[idx, D] = knnsearch(saddles, q);
		if D == 0
			assignments = [assignments; T, i, idx]; 
		end
	end	
end

function [T,q, sl, assignments] = slope_line(Ix, Iy, saddle_id, q, s, nrows, ncols, saddles, mins, maxs, assignments, c)
	hold on		
	pFx = 0;
	pFy = 0;
	sl = java.util.LinkedList;
	if s > 0
		T = 2;
	else
		T = 1;
	end	
	while true		
		plot(q(1),q(2),'.', 'Color', c);
		drawnow
		Fx = interp2(Ix, q(1), q(2));	
		Fy = interp2(Iy, q(1), q(2));
		magF = sqrt(Fx.^2+Fy.^2);
		Fx = Fx ./ magF;
		Fy = Fy ./ magF;			
		q = q + s*[Fx Fy];
		sl.add(q);
		% if arrive at the image border
		if q(1) < 1 || q(2) < 1 || q(1) > ncols || q(2) > nrows 
			q(1) = max(1,min(ncols, q(1)));
			q(2) = max(1,min(nrows, q(2)));
			%T = 0;
			break
		end
		% if arrive at another saddle point
		[cp_idx, D] = knnsearch(saddles, q);
		if D <= 1 && cp_idx ~= saddle_id
			T = 3;	
			q = saddles(cp_idx,:);		
			break;
		end
		% if converged
		if T == 1
		    [cp_idx, D] = knnsearch(mins, q);
		    qq = mins(cp_idx,:);
		else
		    [cp_idx, D] = knnsearch(maxs, q);
		    qq = maxs(cp_idx,:);
		end
		if D <= 1
		    q = qq;
		    break
		end
		%if abs(1 + pFx*Fx + pFy*Fy) < 1e-6 || magF == 0					
		%if magF < .5
		%	break			
		%end			
		pFx = Fx;
		pFy = Fy;
	end
	hold off
	assignments = [assignments; T, saddle_id, cp_idx];
	sl = reshape(cell2mat(cell(sl.toArray(sl))), 2, sl.size())';
end
