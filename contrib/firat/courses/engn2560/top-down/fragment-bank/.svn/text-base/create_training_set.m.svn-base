function create_training_set(datasetpath, gtpath, img_ext, training_indices, Nfrags, totNfrags, minArea, maxArea, outfolder)
        if exist(outfolder)~=7
                mkdir(outfolder);
        end 
        imgfiles = dir([datasetpath '/*.' img_ext]);
        trainfiles = imgfiles(training_indices);
        fid = fopen([outfolder '/info.txt'], 'w');        
        for i = 1:length(trainfiles)
                extract_random_fragments([datasetpath '/' trainfiles(i).name], [gtpath '/' trainfiles(i).name], Nfrags, minArea, maxArea, outfolder);
                fprintf(fid, '%s\n', [datasetpath '/' trainfiles(i).name]);
        end
        fclose(fid); 
        
        postprocess_fb(outfolder, totNfrags, 4);       
end

function postprocess_fb(fb, totNfrags, nbins)
        files = dir([fb '/*.mat']);
        all_centers = zeros(length(files),2);
        
        for i = 1:length(files)
                load([fb '/' files(i).name]);
                all_centers(i,:) = normalized_center;
        end
        [idx, C] = kmeans(all_centers, nbins);
        h = hist(idx, 1:nbins);
        minval = totNfrags / nbins; 
        figure;bar(h)
        for i = 1:nbins
                members = find(idx == i);
                RP = randperm(h(i));
                ToDel = RP((minval+1):end);
                ToDelFiles = files(members(ToDel));
                for j = 1:length(ToDelFiles)
                        eval(sprintf('!rm %s/%s', fb, ToDelFiles(j).name));                        
                end
        end      
        
        
end

