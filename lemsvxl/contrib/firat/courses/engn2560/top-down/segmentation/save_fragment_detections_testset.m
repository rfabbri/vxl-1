function save_fragment_detections_testset(datasetpath, fragbank, img_ext, test_indices, maxNdet, outfolder)
        if exist(outfolder)~=7
                mkdir(outfolder);
        end 
        imgfiles = dir([datasetpath '/*.' img_ext]);
        testfiles = imgfiles(test_indices);
        for i = 1:length(testfiles)
                imfile = [datasetpath '/' testfiles(i).name];
                tic;all_detections = detect_all_fragments(imfile, fragbank, maxNdet);toc
                savefile = [outfolder '/' get_obj_name(imfile) '_frag_detect.mat'];
                save(savefile, 'all_detections');                
        end
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end
