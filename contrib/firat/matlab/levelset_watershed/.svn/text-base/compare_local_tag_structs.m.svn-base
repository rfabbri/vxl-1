function compare_local_tag_structs(img_folder, tag_folder, img1, img2, sigma, crit_id1, crit_id2)
	I1 = double(rgb2gray(imread([img_folder '/' img1])));
	I2 = double(rgb2gray(imread([img_folder '/' img2])));	
	X1 = load([tag_folder '/' img1 '.mat']);
	X2 = load([tag_folder '/' img2 '.mat']);
	
	show_critical_points(I1, sigma, X1.mins, X1.maxs, X1.saddles);
	show_critical_points(I2, sigma, X2.mins, X2.maxs, X2.saddles);
	
	%plot_tag(I1, sigma, X1.mins, X1.maxs, X1.saddles, X1.saddle_links)
	%plot_tag(I2, sigma, X2.mins, X2.maxs, X2.saddles, X2.saddle_links)
	
	
	figure;show_local_tag_struct(I1, sigma, X1.mins, X1.maxs, X1.saddles, X1.min_links, X1.max_links, X1.saddle_links, crit_id1)
	figure;show_local_tag_struct(I2, sigma, X2.mins, X2.maxs, X2.saddles, X2.min_links, X2.max_links, X2.saddle_links, crit_id2)
end
