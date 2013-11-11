% This is /lemsvxl/contrib/firat/matlab/select_gt_object.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 19, 2010

function select_gt_object(gt_folder, image_path, save_mask, save_bb, save_cont)
    addpath contour_related
    if ~exist(gt_folder, 'dir')
        mkdir(gt_folder)
    end
    objname = get_objectname_from_filename(image_path);
    bb_filename = [gt_folder '/' objname '.groundtruth'];
    contour_filename = [gt_folder '/' objname '.txt'];
    k = 0;
    mask_filename = [gt_folder '/' objname '.mask.'  num2str(k) '.png'];
    while exist(mask_filename, 'file')
        k = k + 1;
        mask_filename = [gt_folder '/' objname '.mask.'  num2str(k) '.png'];
    end
    I = imread(image_path);
    
    imshow(I);
    hold on;
    
    [x(1),y(1),b] = ginput(1);
    plot(x(1),y(1),'rd');
    
    [x(2),y(2),b] = ginput(1);
        
    plot([x(1) x(2)], [y(1) y(2)], 'rd-')
    t = 3;
    while b == 1
        [x(t),y(t),b] = ginput(1);
        plot([x(t-1) x(t)], [y(t-1) y(t)], 'rd-');
        t = t + 1;    
    end  
    plot([x(t-1) x(1)], [y(t-1) y(1)], 'rd-');  
    hold off;    
    bw = poly2mask(x, y, size(I,1), size(I,2));
    figure; imagesc(bw)
    if save_mask
        imwrite(bw,mask_filename);
    end
    if save_bb
        props1 = regionprops(double(bw), 'BoundingBox');
        bb = props1.BoundingBox;
        bb2 = round([bb(1) bb(2) (bb(1)+bb(3)-1) (bb(2)+bb(4)-1)]);
        dlmwrite(bb_filename, bb2, 'delimiter', ' ', '-append');
    end
    if save_cont
        save_contours({[x;y]'}, contour_filename);
    end
end
