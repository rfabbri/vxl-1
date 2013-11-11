% This is recons_level.m
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 7, 2012

function R = recons_level(I, mins, maxs, saddles, slope_lines, assignments, mode)
	[nrows,ncols] = size(I);
	if mode == 1 % use mins, maxs only
		if isempty(mins)
			P = maxs;
		elseif isempty(maxs)
			P = mins;
		else
			P = [mins; maxs];
		end
		
		index = sub2ind([nrows ncols], round(P(:,2)), round(P(:,1)));						
	elseif mode == 2
		%index = sub2ind([nrows ncols], round([mins(:,2);maxs(:,2); ...
		%		saddles(:,2)-1; saddles(:,2)-1; saddles(:,2)-1; ...
		%		saddles(:,2); saddles(:,2); saddles(:,2); ...
		%		saddles(:,2)+1; saddles(:,2)+1; saddles(:,2)+1]),...
		%		round([mins(:,1);maxs(:,1);...
		%		saddles(:,1)-1;saddles(:,1);saddles(:,1)+1;...
		%		saddles(:,1)-1;saddles(:,1);saddles(:,1)+1;...
		%		saddles(:,1)-1;saddles(:,1);saddles(:,1)+1;...				
		%		]));
		index = sub2ind([nrows ncols], round([mins(:,2);maxs(:,2);saddles(:,2)]), round([mins(:,1);maxs(:,1);saddles(:,1)]));
	elseif mode == 3
		index = [];
		for i = 1:length(slope_lines)
			II = round(slope_lines{i}(1:end,2));
			JJ = round(slope_lines{i}(1:end,1));
			in = find(II >= 1 & II <= nrows & JJ >= 1 & JJ <= ncols);
			II = II(in);
			JJ = JJ(in);
			index = [index; sub2ind([nrows ncols], II, JJ)];
		end
	end
	K = zeros(nrows, ncols);
	K(:,1) = 1;
	K(:,end) = 1;
	K(1,:) = 1;
	K(end,:) = 1;
	index = [index; find(K)];
	index = unique(index);
	value = I(index);
	MA = max(I(:));
	MI = min(I(:));
	
	%A = ones(nrows,ncols)*-5;
	%phi = (MA-MI)*rand(nrows, ncols) + MI;
	phi = I;
	%A = I;
	phi(index) = value;
	figure;
	subplot(1,3,1);imagesc(I); colormap gray; axis image; axis off; title('original');
	subplot(1,3,2);imagesc(phi, [min(I(:)) max(I(:))]); colormap gray; axis image; axis off; title('random initialization')
	beta1 = -.01;	
	for i = 1:10000
		phi = levelset2dupdate(phi, 1, 'graddt', {}, 0, beta1, 0, 1, 1, 1, 0, 1, 0);
		phi(index) = value;
		
		
	end
	subplot(1,3,3);imagesc(phi); colormap gray; axis image; axis off
	title('generated image')
	R = phi;
end
