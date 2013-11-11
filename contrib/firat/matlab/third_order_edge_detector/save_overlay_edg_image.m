% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector/save_overlay_edg_image.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 29, 2011

function save_overlay_edg_image(edgeindir, imageindir, outdir)
	addpath ..
	if ~exist(outdir, 'dir')
		mkdir(outdir);
	end
	files = dir([edgeindir '/*.edg']);
	for i = 1:length(files)
		edgfile = [edgeindir '/' files(i).name];
		[edg, edgemap] = load_edg(edgfile);
		objname = get_objectname_from_filename(edgfile);
		I = imread([imageindir '/' objname '.png']);
		imagesc(I); colormap gray; axis off; axis image;
		disp_edg(edg, 'r');
		saveas(gcf, [outdir '/' objname '_3rd.png']);
		close all;
	end

end
