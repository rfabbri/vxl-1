% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/draw_contours_on_slices.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 20, 2011

function draw_contours_on_slices(vtk_file, contour_folder)
    V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/(Max-Min);
	outfolder = contour_folder;
	for i = 1:size(V,3)
	    contours = load_contours(fullfile(contour_folder, [num2str(i) '.txt']));
	    imagesc(V(:,:,i), [0 255]); colormap gray; axis image; axis off;
	    visualize_contours(contours);
	    saveas(gcf, [outfolder '/' num2str(i) '.png']);
	    close all
	end

end
