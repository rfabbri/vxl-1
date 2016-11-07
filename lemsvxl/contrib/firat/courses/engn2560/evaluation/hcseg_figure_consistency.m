function fc = hcseg_figure_consistency(hcseg_folder, gt_folder, gt_ext)
        files = dir([hcseg_folder '/*.mat']);
        fcvec = zeros(length(files), 1);
        for i = 1:length(files)
                filename = [hcseg_folder '/' files(i).name];
                load(filename);
                objname = get_obj_name(filename);
                objname = objname(1:end-10);
                gt_file = [gt_folder '/' objname gt_ext];
                GT = imread(gt_file);
                GT = double(GT > max(GT(:))/2);
                fcvec(i) = figure_consistency(hcsg > 0, GT);               
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
