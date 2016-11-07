function extract_regular_sift(data_path, img_ext, outfolder)
    files = dir([data_path '/*' img_ext]);
	if ~exist(outfolder,'dir')
		mkdir(outfolder);
	end
	for i = 1:length(files)
		I = imread([data_path '/' files(i).name]);		
		[reg_f,reg_d] = vl_sift(single(rgb2gray(I)));
		save([outfolder '/' files(i).name '.mat'], 'reg_f', 'reg_d');
	end
end
