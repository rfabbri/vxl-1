function run_fake_td(gt_path, gt_ext, outfolder)
        if exist(outfolder) ~= 7
                mkdir(outfolder);
        end
        files = dir([gt_path '/*.' gt_ext]);
        for i = 1:length(files)
                gtfile = [gt_path '/' files(i).name];
                GT = imread(gtfile);
                objname = get_obj_name(gtfile);
                td = double(GT > max(GT(:))/2);
                outfile = [outfolder '/' objname '_td.mat'];
                save(outfile, 'td');
        end
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end
