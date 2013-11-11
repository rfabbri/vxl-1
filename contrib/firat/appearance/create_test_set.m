% This is /lemsvxl/contrib/firat/appearance/create_test_set.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 9, 2010

function create_test_set(class_folders, test_indices, img_ext, class_names, desc_type, parameters, test_out, query_files_dir, num_processes)
    fid = zeros(1, num_processes);
    for i = 1:num_processes
        fid(i) = fopen(sprintf('%s/query%d.txt',query_files_dir,i), 'w');
    end
    k = 0;
    for i = 1:length(class_names)        
        for j = test_indices{i}
            [desc, files] = collect_descriptors_of_dataset(class_folders{i}, img_ext, j, desc_type, parameters);
            if isempty(desc)
                continue
            end
            outfile = [test_out '/' class_names{i} '_' files.name '_' desc_type '_' num2str(size(desc, 2)) '_test.pts'];
            dlmwrite(outfile, desc, 'delimiter', ' ');
            t = mod(k, length(fid)) + 1;
            fprintf(fid(t), '%s\n', outfile);
            k = k + 1;
        end
    end        
    for i = 1:length(fid)
        fclose(fid(i)); 
    end
end

