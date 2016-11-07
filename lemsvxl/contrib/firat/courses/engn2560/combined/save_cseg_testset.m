function save_cseg_testset(datasetpath, tdpath, bupath, outfolder, test_indices, bulevel, ov_thres)
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
                bu_out = [bupath '/' objname '/' objname '_0000' num2str(bulevel) '.txt'];
                csg = cseg(bu_out, td_out, ov_thres);
                save([outfolder '/' objname '_combined.mat'], 'csg');
        end

end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end
