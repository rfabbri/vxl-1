function tdseg2(datasetpath, detectionpath, outfolder, id, coeff, minscore)
        files = dir(datasetpath);
        files = files(3:end);
        imfile = [datasetpath '/' files(id).name];
        objname = get_obj_name(imfile);
        detfile = [detectionpath '/' objname '_frag_detect.mat'];
        td = tdseg(imfile, detfile, coeff, minscore);
        if exist(outfolder) ~= 7
                mkdir(outfolder);
        end
        outfile = [outfolder '/' objname '_td.mat'];
        %figure;imagesc(td)
        save(outfile, 'td');
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end
