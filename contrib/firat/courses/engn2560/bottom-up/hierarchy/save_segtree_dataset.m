function save_segtree_dataset(datasetpath, img_ext, test_indices, bu_folder, nbins, outfolder, saliency_measure, npad)
        if exist(outfolder)~=7
                mkdir(outfolder);
        end 
        imgfiles = dir([datasetpath '/*.' img_ext]);
        testfiles = imgfiles(test_indices);
        for i = 1:length(testfiles)
                imfile = [datasetpath '/' testfiles(i).name];
                outfile = [outfolder '/' get_obj_name(imfile) '_segtree.mat'];
                tic;create_hierarchy(imfile, bu_folder, nbins, outfile, saliency_measure, npad);toc                
        end
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end

