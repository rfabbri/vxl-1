% This is vlfeatmat_crit2keygz.m.

% Created on: Apr 15, 2013
%     Author: firat

function vlfeatmat_crit2keygz(mat_folder, minkeygz_folder, maxkeygz_folder, saddlekeygz_folder, percentage)
    files = dir([mat_folder '/*.mat']);
    if ~exist(minkeygz_folder,'dir')
        mkdir(minkeygz_folder);
    end
    if ~exist(maxkeygz_folder,'dir')
        mkdir(maxkeygz_folder);
    end
    if ~exist(saddlekeygz_folder,'dir')
        mkdir(saddlekeygz_folder);
    end
    for i = 1:length(files)
        X = load([mat_folder '/' files(i).name]);
        dotpos = find(files(i).name == '.', 1);
        basename = files(i).name(1:dotpos);
        keygzname = [basename 'key']; 
        
        num_min = size(X.min_d, 2); 
        num_min_to_keep = round(num_min * percentage);  
        [ign, min_sort_index] = sort(X.min_s_min, 'descend');
        
        num_max = size(X.max_d, 2); 
        num_max_to_keep = round(num_max * percentage);  
        [ign, max_sort_index] = sort(X.max_s_min, 'descend');
        
        num_saddle = size(X.saddle_d, 2); 
        num_saddle_to_keep = round(num_saddle * percentage);  
        [ign, saddle_sort_index] = sort(X.saddle_s_min, 'descend');
            
        vl_ubcwrite(X.min_f(:, min_sort_index(1:num_min_to_keep)), X.min_d(:, min_sort_index(1:num_min_to_keep)), [minkeygz_folder '/' keygzname], true); 
        vl_ubcwrite(X.max_f(:, max_sort_index(1:num_max_to_keep)), X.max_d(:, max_sort_index(1:num_max_to_keep)), [maxkeygz_folder '/' keygzname], true);
        vl_ubcwrite(X.saddle_f(:, saddle_sort_index(1:num_saddle_to_keep)), X.saddle_d(:, saddle_sort_index(1:num_saddle_to_keep)), [saddlekeygz_folder '/' keygzname], true);
    end
end

