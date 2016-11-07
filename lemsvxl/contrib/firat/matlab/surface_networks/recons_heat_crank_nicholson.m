% This is recons_heat_crank_nicholson.m
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 14, 2012

function R = recons_heat_crank_nicholson(I, mins, maxs, saddles, slope_lines, assignments, mode)
	[nrows, ncols, index, value, A] = initialize_recons_heat(I, mins, maxs, saddles, slope_lines, assignments, mode);	
	CNleft = zeros(nrows*ncols, nrows*ncols);
	CNright = zeros(nrows*ncols, nrows*ncols);	
	L = 1;
	
	for i = 1:nrows
		for j = 1:ncols
			ind1 = sub2ind([nrows ncols], i, j);
			if any(ind1 == index) 
				CNleft(ind1,ind1) = 1;
				CNright(ind1,ind1) = 1;	
			else
				[jgrid, igrid] = meshgrid(j-1:j+1, i-1:i+1);
				ind = sub2ind([nrows ncols], igrid, jgrid);
				CNleft(ind(2,2), ind(2,2)) = 1+4*L;		
				CNleft(ind(2,2), ind(1,2)) = -L;
				CNleft(ind(2,2), ind(2,1)) = -L;
				CNleft(ind(2,2), ind(2,3)) = -L;
				CNleft(ind(2,2), ind(3,2)) = -L;
				
				CNright(ind(2,2), ind(2,2)) = 1-4*L;		
				CNright(ind(2,2), ind(1,2)) = L;
				CNright(ind(2,2), ind(2,1)) = L;
				CNright(ind(2,2), ind(2,3)) = L;
				CNright(ind(2,2), ind(3,2)) = L;							
			end
		end
	end
	
	%figure;
	%subplot(1,3,1);imagesc(I); colormap gray; axis image; axis off; title('original');
	%subplot(1,3,2);imagesc(A, [min(I(:)) max(I(:))]); colormap gray; axis image; axis off; title('random initialization')	
	M = inv(CNleft)*CNright;
	v = (M^1000)*A(:);
	R = reshape(v, [nrows, ncols]);
	%subplot(1,3,3);
	imagesc(R, [min(I(:)) max(I(:))]); colormap gray; axis image; axis off
	%title('Reconstruction')
	%for i = 1:1000
	%	v = CNleft\(CNright*A(:));
	%	A = reshape(v, [nrows, ncols]);
	%	subplot(1,3,3);imagesc(A, [min(I(:)) max(I(:))]); colormap gray; axis image; axis off	
	%	drawnow	
	%end
	%R = A;


	
end
