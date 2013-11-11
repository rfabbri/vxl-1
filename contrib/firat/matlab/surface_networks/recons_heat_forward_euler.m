% This is recons_heat.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 24, 2012

function R = recons_heat_forward_euler(I, mins, maxs, saddles, slope_lines, assignments, mode)
	[nrows, ncols, index, value, A] = initialize_recons_heat(I, mins, maxs, saddles, slope_lines, assignments, mode);
	figure;
	subplot(1,3,1);imagesc(I); colormap gray; axis image; axis off; title('original');
	subplot(1,3,2);imagesc(A, [min(I(:)) max(I(:))]); colormap gray; axis image; axis off; title('random initialization')
	L = .125;
	h = [0 L 0; L (1-4*L) L; 0 L 0];
	for i = 1:10000
		A = imfilter(A, h, 'replicate');
		A(index) = value;		
		drawnow
	end
	subplot(1,3,3);imagesc(A, [min(I(:)) max(I(:))]); colormap gray; axis image; axis off
	title('generated image')
	R = A;
	
	
	
end
