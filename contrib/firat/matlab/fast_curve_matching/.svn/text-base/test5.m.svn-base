% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/test5.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 9, 2011

clear
close all

image_name = 'bottles_tobias';

output_base_folder = '/home/firat/Desktop/db/results';


obj_contour = load_contours('/home/firat/Desktop/db/1000bottles/bottle1.txt');
query_contours = load_contours(['/home/firat/Desktop/db/bottle_contours/' image_name '.txt']);


mkdir([output_base_folder '/' image_name]);

for it = 1:length(query_contours)
    query_curve = query_contours{it}; %6
    query_image = ['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/' image_name '.jpg'];
    test_curve = obj_contour{1};
    F = query_curve;
    max_num_samples = 80;
    num_rows = 5;
    num_cols = 7;
    num_detection = num_rows * num_cols;

    I = imread(query_image);


    tic;
    %[cf_center, cf_len] = scale_invariant_match(test_curve, F, max_num_samples, 3);
    %[cf_center, cf_len] = scale_invariant_match2(test_curve, F, max_num_samples, 3, num_detection);
    %[cf_center, cf_len] = scale_invariant_match3(test_curve, F, max_num_samples, 3, num_detection);
    [cf_center, cf_len, cf_rev] = scale_invariant_match4(test_curve, F, max_num_samples, 3, num_detection);
    toc


    if 0
    figure;
    for i = 1:length(cf_center)
        j = cf_center(i)/max_num_samples*size(test_curve,1);
        l = cf_len(i)/max_num_samples*size(test_curve,1);
        MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;    
        subplot(num_rows,num_cols,i)
        imshow(ones(1,1)); hold on
        plot(test_curve(:,1),test_curve(:,2)); axis off; axis image
        plot(test_curve(MF,1),test_curve(MF,2), 'g', 'LineWidth', 3)
        hold off;
    end
    end



    figure;
    for i = 1:length(cf_center)
        
        j = cf_center(i)/max_num_samples*size(test_curve,1);
        l = cf_len(i)/max_num_samples*size(test_curve,1);
        MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;
        
        S2 = sample_curve(F, 3, length(MF));
        S3 = test_curve(MF,:);   
        
        if cf_rev(i)
            T = cp2tform(S3(end:-1:1,:),S2,'nonreflective similarity');
        else
            T = cp2tform(S3,S2,'nonreflective similarity');
        end
        [tCurvex, tCurvey] = tformfwd(T, test_curve(:,1), test_curve(:,2));
        
        
        subplot(num_rows,num_cols,i)
        imshow(I);
        hold on; plot(tCurvex,tCurvey, 'g', 'LineWidth', 2)
        plot(query_curve(:,1),query_curve(:,2), 'r-','LineWidth', 2);
        hold off;
        
        
    end

    saveas(gcf, [output_base_folder '/' image_name '/' image_name '_' num2str(it) '.png'], 'png');
    close all
    
    
    
    if 0
    figure;
    for i = 1:length(cf_center)
        j = cf_center(i)/max_num_samples*size(test_curve,1);
        l = cf_len(i)/max_num_samples*size(test_curve,1);
        MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;
        
        S2 = sample_curve(F, 3, length(MF));
        S3 = test_curve(MF,:);   
        
        T = cp2tform(S3(end:-1:1,:),S2,'nonreflective similarity');
        
        [tCurvex, tCurvey] = tformfwd(T, test_curve(:,1), test_curve(:,2));
        
        
        subplot(num_rows,num_cols,i)
        imshow(I);
        hold on; plot(tCurvex,tCurvey, 'g', 'LineWidth', 3)
        plot(query_curve(:,1),query_curve(:,2), 'r-','LineWidth', 3);
        hold off;
    end
    end
    %figure;
    %for i = 1:length(cf_center)
    %    j = cf_center(i)/max_num_samples*size(test_curve,1);
    %    l = cf_len(i)/max_num_samples*size(test_curve,1);
    %    MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;
    %    %MF = round(j-l/2):round(j+l/2);
    %    %S2 = sample_curve(F, 3, length(MF));
    %    S3 = test_curve(MF,:);   
        
           
    %    subplot(5,6,i)
    %    hold on
end
