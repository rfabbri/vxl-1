function extract_class_images(image_folder, annotation_folder, class_name, new_ds, new_gt, gt_ext)
    files = dir([annotation_folder '/*.xml']);
    if ~exist(new_ds, 'dir')
        mkdir(new_ds);
    end
    if ~exist(new_gt, 'dir')
        mkdir(new_gt);
    end
    for i = 1:length(files)
        annofile = [annotation_folder '/' files(i).name];
        rec = PASreadrecord(annofile);
        bbs = [];
        for j = 1:length(rec.objects)            
            if strcmp(class_name, rec.objects(j).class)
                bbs = [bbs; rec.objects(j).bbox];
            end
        end
        if size(bbs,1) > 0
            outimfile = [new_ds '/' class_name '_' rec.filename];
            srcfile = [image_folder '/' rec.filename];
            objname = get_objectname_from_filename(srcfile);
            outbbfile = [new_gt '/' class_name '_' objname '.' gt_ext];
            copyfile(srcfile, outimfile);
            dlmwrite(outbbfile, bbs, 'delimiter', ' ');
            
        end
    end

end
