function fc = cseg_overall_consistency(cseg_folder, gt_folder, gt_ext)
        files = dir([cseg_folder '/*.mat']);
        total = 0;
        fcvec = zeros(length(files), 1);
        for i = 1:length(files)
                filename = [cseg_folder '/' files(i).name];
                load(filename);
                objname = get_obj_name(filename);
                objname = objname(1:end-9);
                gt_file = [gt_folder '/' objname gt_ext];
                GT = imread(gt_file);
                GT = double(GT > max(GT(:))/2);
                fcvec(i) = overall_consistency(csg > 0, GT);
                total = total + fcvec(i);
        end
        fc = mean(fcvec);
        [maxc, maxin] = max(fcvec);
        fprintf('Max performance: %s\n', files(maxin).name);
        [minc, minin] = min(fcvec); 
        fprintf('Min performance: %s\n', files(minin).name);
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end
