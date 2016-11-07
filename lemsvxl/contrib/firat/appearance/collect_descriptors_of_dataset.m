function [desc, files] = collect_descriptors_of_dataset(ds_path, img_ext, file_indices, desc_type, parameters)    
    %%for a new descriptor, only change this part
    if strcmp(desc_type, 'rgb')
        parameter_names = {'nrows', 'ncols'};  
    elseif strcmp(desc_type, 'lab')
        parameter_names = {'nrows', 'ncols'};         
    elseif strcmp(desc_type, 'dense_sift')
        parameter_names = {'grad_thresh', 'spatial_scales', 'grid_spacing'};
    elseif strcmp(desc_type, 'gabor')
        parameter_names = {'nscale', 'norient', 'minWaveLength', 'mult', 'sigmaOnf', 'dThetaOnSigma', 'sampling_rate'};
    end
    %% 
    param_count = length(parameters);
    for i = 1:param_count
        eval(sprintf('%s = parameters{%d};', parameter_names{i}, i));
    end  
    
    files = dir([ds_path '/*.' img_ext]);
    desc = cell(length(files), 1);
    command_str = sprintf('desc{i} = %s_descriptor(%%s, ', desc_type);
    for j = 1:param_count-1               
        command_str = [command_str parameter_names{j} ', '];  
    end
    if param_count > 0
        command_str = [command_str parameter_names{param_count} ');'];
    else
        command_str(end-1:end+1) = ' );';
    end
    for i = file_indices
        try   
            %sprintf(command_str, [39 ds_path '/' files(i).name 39])         
            eval(sprintf(command_str, [39 ds_path '/' files(i).name 39]));
        catch
        
        end
    end 
    desc = cell2mat(desc);
    files = files(file_indices);
end
