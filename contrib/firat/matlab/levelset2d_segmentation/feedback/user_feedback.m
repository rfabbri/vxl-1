% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/feedback/user_feedback.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 23, 2011

function user_feedback(segmentation_folder, outfolder, s_index, e_index)
	if ~exist(outfolder, 'dir')
		mkdir(outfolder);	
	end
	close all
	PP = load([segmentation_folder '/parameters.mat']);
	V = PP.V;
	problematic = show_segmentations_for_feedback(segmentation_folder, s_index, e_index);
	for i = 1:length(problematic)
		k = problematic(i);		
		while(1)
			load([segmentation_folder '/' num2str(k) '.mat']);
			show_segmentation_matlab(V(:,:,k), phi, PP.gridx, PP.gridy);
			option1 = input('[1] Edit edge map and re-run segmentation [2] Remove segmentation: ');
			if option1 == 1
				edg = edgemap_correction_tool(V(:,:,k), PP.edge_thresh, PP.edge_sigma);
				phi = autoinit_reaction_geodesic_graddt_edge_stop_bubble(V(:,:,k), PP.beta0, PP.alpha, 1, PP.max_num_iter, PP.hx, PP.hy, 0, 0, PP.reg_mu, PP.reg_sigma, PP.narrow_band, edg, 0);						
			else
				phi = segmentation_removal_tool(V(:,:,k), phi, PP.hx, PP.hy, PP.narrow_band);
			end
			show_segmentation_matlab(V(:,:,k), phi, PP.gridx, PP.gridy);
			option2 = input('Save new segmentation? [1] yes [2] no: ');
			if option2 == 1
				saveas(gcf, [outfolder '/' num2str(k) '.png']);
				save([outfolder '/' num2str(k) '.mat'], 'phi');	
				close all
				break
			end			
		end
	end
end

