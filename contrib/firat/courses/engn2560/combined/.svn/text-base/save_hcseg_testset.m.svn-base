function save_hcseg_testset(datasetpath, tdpath, segtree_path, outfolder, test_indices, ov_thres)
        files = dir(datasetpath);
        files = files(3:end);
        files = files(test_indices);
        if exist(outfolder) ~= 7
                mkdir(outfolder);
        end
        for i = 1:length(files)
                imfile = [datasetpath '/' files(i).name];
                objname = get_obj_name(imfile);
                td_out = [tdpath '/' objname '_td.mat'];
                bu_out = [segtree_path '/' objname '_segtree.mat'];
                hcsg = hcseg(bu_out, td_out, ov_thres);
                save([outfolder '/' objname '_hcombined.mat'], 'hcsg');
        end

end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end
