% This is /lemsvxl/contrib/firat/appearance/create_training_set.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 9, 2010

function create_training_set(class_folders, train_indices, img_ext, class_names, desc_type, parameters, train_out)
    for i = 1:length(class_names)
        desc = collect_descriptors_of_dataset(class_folders{i}, img_ext, train_indices{i}, desc_type, parameters);
        dlmwrite([train_out '/' class_names{i} '_' desc_type '_' num2str(size(desc, 2)) '_train.pts'], desc, 'delimiter', ' ');
    end
end
