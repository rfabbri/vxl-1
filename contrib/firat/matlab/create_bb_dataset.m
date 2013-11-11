function create_bb_dataset(orig_ds, bb_gt_folder, img_ext, gt_ext, bb_ds, new_img_ext, new_gt_folder)
    files = dir([orig_ds '/*.' img_ext]);
    if ~exist(bb_ds, 'dir')
        mkdir(bb_ds);
    end
    if ~exist(new_gt_folder)
        mkdir(new_gt_folder);
    end
    for i = 1:length(files)
        img_file = [orig_ds '/' files(i).name];
        objname = get_objectname_from_filename(img_file, img_ext);
        bb_file = [bb_gt_folder '/' objname '.' gt_ext];
        bb_all = load(bb_file);
        for j = 1:size(bb_all,1)
            bb = bb_all(j,:);
            w = bb(3) - bb(1) + 1;
            h = bb(4) - bb(2) + 1;
            I = imread(img_file);
            IC = imcrop(I, [bb(1:2) w h]);
            newbb = [1 1 w+1 h+1];
            out_imfile = [bb_ds '/' objname '_bb' num2str(j) '.' new_img_ext];
            out_gtfile = [new_gt_folder '/' objname '_bb' num2str(j) '.' gt_ext];
            imwrite(IC, out_imfile);
            dlmwrite(out_gtfile, newbb, 'delimiter', ' ');
        end
    end
end
