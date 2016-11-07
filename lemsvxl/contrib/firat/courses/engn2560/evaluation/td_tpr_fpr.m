function fc = td_tpr_fpr(td_folder, gt_folder, gt_ext)
        files = dir([td_folder '/*.mat']);
        fcvec = zeros(length(files), 2);
        for i = 1:length(files)
                filename = [td_folder '/' files(i).name];
                load(filename);
                objname = get_obj_name(filename);
                objname = objname(1:end-3);
                gt_file = [gt_folder '/' objname gt_ext];
                GT = imread(gt_file);
                GT = double(GT > max(GT(:))/2);
                [fcvec(i,1), fcvec(i,2)] = tpr_fpr(td > 0, GT);                
        end
        fc = mean(fcvec);        
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end
