% This is show_slope_districts_with_shock_graphs.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 2, 2012

function [WS,WC] = show_slope_districts_with_shock_graphs(I, cem_file, esf_file, sigma, show_contours, show_shocks, show_districts)
	addpath /home/firat/Downloads/shock_visualization
	
	[nrows,ncols] = size(I);
	if sigma > 0
		G = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		S = imfilter(I,G,'replicate');
	else
		S = I;
	end
	
	%plot(ws_c, ws_r, '.', 'MarkerEdgeColor', [139, 0, 255]/255);
	%plot(wc_c, wc_r, '.', 'MarkerEdgeColor', [255 160 0]/255);
	figure;
	imagesc(S); colormap gray; axis image; axis off	
	hold on		
	if show_shocks
		[shock_samples,shock_edges]=read_shock_file(esf_file);		
		plot_shock_graph(shock_samples,shock_edges,'g');
	end
	if show_contours
		[contours]=read_cem_file(cem_file);
		draw_contours(contours, 0, 0, 'r');
	end
	if show_districts
		WS = watershed(S, 8); %watershed = max + saddle + ridges
		WC = watershed(-S, 8); %watercourse = min + saddle + valleys
		[ws_r, ws_c] = find(WS == 0);
		[wc_r, wc_c] = find(WC == 0);
	
		maxK = imregionalmax(S);
		[max_r,max_c] = find(maxK);
		minK = imregionalmin(S);
		[min_r,min_c] = find(minK);
			
		trace_discrete_watershed(WS == 0, [139, 0, 255]/255);
		trace_discrete_watershed(WC == 0, [255 160 0]/255);
		hold on
		plot(min_c, min_r, 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
		plot(max_c, max_r, '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);	
	end
	hold off
	
	if 0
		clear
		close all
		%objnames = {'regular','near-regular','stochastic', 'near-stochastic', 'irregular'};
		objnames = {'1','2','3', '4', '5', '6', '7', '8', '9'};
		%input_folder = '/home/firat/Desktop/textures';
		%output_folder = '/home/firat/Desktop/textures/sd-contour-output';	
		input_folder = '/home/firat/Desktop/natural';
		output_folder = '/home/firat/Desktop/natural/sd-contour-output';	
		sigmas = 1:2;
		for i = 1:length(objnames)
			objname = objnames{i};
			filename = sprintf('%s/%s.jpg', input_folder, objname);
			I = double(rgb2gray(imread(filename)));
			cem_file = sprintf('%s/%s.cemv',input_folder, objname);
			esf_file = '';
			for j = 1:length(sigmas)				
				sigma = sigmas(j);				
				show_slope_districts_with_shock_graphs(I, cem_file, esf_file, sigma, true, false, true);
				set(gcf, 'Position', get(0,'Screensize'));
				print_pdf(sprintf('%s/%s_sd_contour_sigma_%d.pdf',output_folder,objname, sigma));
				close all
				if j == 1
					show_slope_districts_with_shock_graphs(I, cem_file, esf_file, sigma, true, false, false);
					set(gcf, 'Position', get(0,'Screensize'));
					print_pdf(sprintf('%s/%s_just_contour_sigma_1.pdf',output_folder,objname));
					close all
				end
				show_slope_districts_with_shock_graphs(I, cem_file, esf_file, sigma, false, false, true);
				set(gcf, 'Position', get(0,'Screensize'));
				print_pdf(sprintf('%s/%s_just_sd_sigma_%d.pdf',output_folder,objname, sigma));
				close all
			end
		end
	end
	
	if 0
		clear
		close all
		addpath ~/Downloads/siftDemoV4
		%objnames = {'regular','near-regular','stochastic', 'near-stochastic', 'irregular'};
		objnames = {'1','2','3', '4', '5', '6', '7', '8', '9'};
		sigmas = 1:2;
		for i = 1:length(objnames)
			objname = objnames{i};
			filename = sprintf('/home/firat/Desktop/textures-small/%s.jpg',objname);
			I = double(rgb2gray(imread(filename)));
			for j = length(sigmas):-1:1
				sigma = sigmas(j);
				L = ceil(6*sigma);
				if mod(L,2) == 0
					L = L + 1;
				end
				H = fspecial('gaussian', [L L], sigma);
				h = fspecial('gaussian', [L 1], sigma);
				J = imfilter(I,H,'replicate');
				
				corners = corner(I, 'Harris', 10^20, 'FilterCoefficients', h);
				[image, descriptors, sifts] = sift(filename);
				maxK = imregionalmax(J); 
				minK = imregionalmin(J); 
				[max_r,max_c] = find(maxK);
				[min_r,min_c] = find(minK);
				figure;
				imagesc(J); colormap gray; axis image; axis off; hold on
				plot(sifts(:,2), sifts(:,1), 'r.', 'MarkerSize', 11);
				plot(corners(:,1), corners(:,2), 'g.', 'MarkerSize', 11);
				plot(min_c, min_r, 'o', 'MarkerEdgeColor',[0, 127, 255]/255);
				plot(max_c, max_r, '^', 'MarkerEdgeColor',[0, 127, 255]/255);
				legend({'sift', sprintf('harris, sigma = %d', sigma), sprintf('min, sigma = %d', sigma), sprintf('max, sigma = %d', sigma)}, 'Location', 'NorthEastOutside');
				set(gcf, 'Position', get(0,'Screensize'));
				print_pdf(sprintf('/home/firat/Desktop/textures-small/texture-keypoints/%s_all_sigma_%d.pdf',objname,sigma));
				figure;
				imagesc(J); colormap gray; axis image; axis off; hold on
				plot(sifts(:,2), sifts(:,1), 'r.', 'MarkerSize', 11);
				title('sift')
				set(gcf, 'Position', get(0,'Screensize'));
				print_pdf(sprintf('/home/firat/Desktop/textures-small/texture-keypoints/%s_sift.pdf',objname));
				close all
				figure;
				imagesc(J); colormap gray; axis image; axis off; hold on
				plot(corners(:,1), corners(:,2), 'g.', 'MarkerSize', 11);
				title(sprintf('harris, sigma = %d', sigma));
				set(gcf, 'Position', get(0,'Screensize'));
				print_pdf(sprintf('/home/firat/Desktop/textures-small/texture-keypoints/%s_harris_sigma_%d.pdf',objname, sigma));
				close all
				figure;
				imagesc(J); colormap gray; axis image; axis off; hold on
				plot(min_c, min_r, 'o', 'MarkerEdgeColor',[0, 127, 255]/255);
				title(sprintf('min, sigma = %d', sigma));
				set(gcf, 'Position', get(0,'Screensize'));
				print_pdf(sprintf('/home/firat/Desktop/textures-small/texture-keypoints/%s_min_sigma_%d.pdf',objname, sigma));
				close all
				figure;
				imagesc(J); colormap gray; axis image; axis off; hold on
				plot(max_c, max_r, '^', 'MarkerEdgeColor',[0, 127, 255]/255);
				title(sprintf('max, sigma = %d', sigma));
				set(gcf, 'Position', get(0,'Screensize'));
				print_pdf(sprintf('/home/firat/Desktop/textures-small/texture-keypoints/%s_max_sigma_%d.pdf',objname, sigma));
				close all
			end
		end
		
		
		
	end
end


