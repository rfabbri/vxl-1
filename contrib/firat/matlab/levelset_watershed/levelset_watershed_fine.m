% This is levelset_watershed_fine.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 17, 2013

function phi = levelset_watershed_fine(I, J, sigma, WS)
	addpath ~/lemsvxl/bin/mex
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/tools
	addpath ~/lemsvxl/src/contrib/firat/matlab/topographical
	[Gx, Gy] = gaussian_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	gradI = sqrt(Ix.^2 + Iy.^2);
	[H,W] = size(WS);
	ctrs = contourc(WS, [0, 0]);
	j = 1;
	all_contours = {};
	if ~isempty(ctrs)
		while j <= size(ctrs,2)
			con = ctrs(:, j+1:j+ctrs(2,j))';
			all_contours = [all_contours; con];			
			j = j + ctrs(2,j) + 1;
			
		end
	end
	[gridx, gridy] = meshgrid(1:W, 1:H);
	%phi = mex_poly_dist_trans({all_contours{16},all_contours{32}, all_contours{25}}, H, W, 1, 1);
	if 0
		for i = 1:length(all_contours)
			phi = mex_poly_dist_trans({all_contours{i}}, H, W, 1, 1);
			show_segmentation_matlab(J, phi, gridx, gridy,0)
			title(num2str(i));
		pause
		end
		return
	end
	
	
	
	
	phi = mex_poly_dist_trans({all_contours{8}}, H, W, 1, 1);
	phi_prev = zeros(H, W);
	
	geodesic_input = {-Ix, -Iy};
	alpha = .001;
	beta0 = 0;
	beta1 = 0;
	narrow_band = 2;
	show_segmentation_matlab(J, phi, gridx, gridy,0)
	hold on;
	quiver(Ix, Iy)
	hold off;
	pause
	for i = 1:1000
	%while ~check_stop_condition(phi_prev,phi, 0.001)
		phi_prev = phi;		
		phi = levelset2dupdate(phi, 1, 'graddt', geodesic_input, beta0, beta1, alpha, 1, 1, 1, 0, 0, 1);				
		phi = reinitlevelset_narrow(phi,narrow_band);
		%phi = remove_small_foreground_from_phi(phi, 0.01, narrow_band);
		phi = -remove_small_foreground_from_phi(-phi, 0.01, narrow_band);
		%phi = sussman2d(phi, 1, narrow_band);
	%	pause(.1)
		show_segmentation_matlab(J, phi, gridx, gridy,0)
		hold on;
		quiver(Ix, Iy)
		hold off;
		drawnow
		%i = i + 1
	end
	show_segmentation_matlab(J, phi, gridx, gridy,0)
	
end
