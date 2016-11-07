% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/evaluation/convert_phi_to_binary.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 12, 2011

function convert_phi_to_binary(infolder, outfolder, s_index, e_index)
	num_slices = e_index - s_index + 1;
	first_time = true;
	%load seg	
	for i = s_index:e_index
		Seg = load([infolder '/' num2str(i) '.mat']);
		if first_time
			first_time = false;
			[nrows,ncols] = size(Seg.phi);			
			if ~exist(outfolder,'dir')
				mkdir(outfolder);
			end
		end
		BW = Seg.phi >= 0;
		save([outfolder '/' num2str(i) '.mat'], 'BW');
	end
end
