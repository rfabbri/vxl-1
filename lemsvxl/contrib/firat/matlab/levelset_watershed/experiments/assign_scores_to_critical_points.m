function assign_scores_to_critical_points(img_folder, img_ext, sift_folder)
	addpath ~/lemsvxl/src/contrib/firat/matlab/levelset2d_segmentation/tools
    clean_old_values = false;   
    files = dir([img_folder '/*' img_ext]);
    if clean_old_values
        for i = 1:length(files)
            sift_file = [sift_folder '/' files(i).name '.mat'];
            S = load(sift_file);
            save(sift_file, '-struct', 'S', 'min_f', 'min_d', 'max_f', ...
            'max_d', 'saddle_f', 'saddle_d');  
        end
        return
    end
	sigma = 2;
	for i = 1:length(files)
		i
		I = double(rgb2gray(imread([img_folder '/' files(i).name])));
		[Gxx, Gyy, Gxy] = gaussian_second_derivative_filter(sigma, 1, 1);
		Ixx = imfilter(I, Gxx, 'replicate', 'conv');
		Iyy = imfilter(I, Gyy, 'replicate', 'conv');
		Ixy = imfilter(I, Gxy, 'replicate', 'conv');
		
		sift_file = [sift_folder '/' files(i).name '.mat'];
		X = load(sift_file);
		
		fxx = interp2(Ixx, X.min_f(1,:), X.min_f(2,:), 'cubic');
		fxy = interp2(Ixy, X.min_f(1,:), X.min_f(2,:), 'cubic');
		fyy = interp2(Iyy, X.min_f(1,:), X.min_f(2,:), 'cubic');
		%II = interp2(I, X.min_f(1,:), X.min_f(2,:), 'cubic');
		A = fxx + fyy;
		B = (fxx.*fyy) - (fxy.^2);
		L1 = (A + sqrt(A.^2 - 4*B))/2;
		L2 = (A - sqrt(A.^2 - 4*B))/2;
        min_s_min = min(abs([L1;L2])); %min curvature
        min_s_gauss = abs(B); %Gaussian curvature
        min_s_mean = abs(A); %mean curvature
		
		fxx = interp2(Ixx, X.max_f(1,:), X.max_f(2,:), 'cubic');
		fxy = interp2(Ixy, X.max_f(1,:), X.max_f(2,:), 'cubic');
		fyy = interp2(Iyy, X.max_f(1,:), X.max_f(2,:), 'cubic');
		%II = interp2(I, X.max_f(1,:), X.max_f(2,:), 'cubic');
		A = fxx + fyy;
		B = (fxx.*fyy) - (fxy.^2);
		L1 = (A + sqrt(A.^2 - 4*B))/2;
		L2 = (A - sqrt(A.^2 - 4*B))/2;
        max_s_min = min(abs([L1;L2])); %min curvature
        max_s_gauss = abs(B); %Gaussian curvature
        max_s_mean = abs(A); %mean curvature
		
		fxx = interp2(Ixx, X.saddle_f(1,:), X.saddle_f(2,:), 'cubic');
		fxy = interp2(Ixy, X.saddle_f(1,:), X.saddle_f(2,:), 'cubic');
		fyy = interp2(Iyy, X.saddle_f(1,:), X.saddle_f(2,:), 'cubic');
		%II = interp2(I, X.saddle_f(1,:), X.saddle_f(2,:), 'cubic');
		A = fxx + fyy;
		B = (fxx.*fyy) - (fxy.^2);
		L1 = (A + sqrt(A.^2 - 4*B))/2;
		L2 = (A - sqrt(A.^2 - 4*B))/2;
        saddle_s_min = min(abs([L1;L2])); %min curvature
        saddle_s_gauss = abs(B); %Gaussian curvature
        saddle_s_mean = abs(A); %mean curvature
		
        save(sift_file, 'min_s_min', 'max_s_min', 'saddle_s_min', ...
                        'min_s_gauss', 'max_s_gauss', 'saddle_s_gauss',...
                        'min_s_mean', 'max_s_mean', 'saddle_s_mean', ...
                        '-append');
	end
end
