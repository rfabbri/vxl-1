% This is extract_random_patch_pairs.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 11, 2012

function extract_random_patch_pairs(I, J, M, N, K, outfolder)
	if ~exist(outfolder,'dir')
		mkdir(outfolder);
	end
	[nrows,ncols,nbands] = size(I);	
	for k = 1:K
		x0 = unidrnd(ncols-N+1);
		y0 = unidrnd(nrows-M+1);
		IC = imcrop(I, [x0 y0 N-1 M-1]);
		JC = imcrop(J, [x0 y0 N-1 M-1]);
		imwrite(IC, fullfile(outfolder, ['I' num2str(k) '.png']));
		imwrite(JC, fullfile(outfolder, ['J' num2str(k) '.png']));
	end
	
end
