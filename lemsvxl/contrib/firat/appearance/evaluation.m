% This is /lemsvxl/contrib/firat/appearance/evaluation.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 9, 2010

function R = evaluation(results_dir)
    files = dir([results_dir '/*.txt']);
    R = struct;
    for i = 1:length(files)
        filename = files(i).name;
        class_name = strtok(filename, '_');
        tot_field = [class_name '_total'];
        cor_field = [class_name '_correct'];
        rate_field = [class_name '_rate'];
        if ~isfield(R, tot_field)            
            R = setfield(R, tot_field, 1);
            R = setfield(R, cor_field, 0);
            R = setfield(R, rate_field, 0);
            total_num = 1;
        else
            n = getfield(R, tot_field);
            total_num = n+1;
            R = setfield(R, tot_field, total_num);            
        end
        [class_labels, class_score] = textread([results_dir '/' filename], '%s %f');
        
        [minval, mindex] = min(class_score);
        n = getfield(R, cor_field);
        if strcmp(class_labels{mindex}, class_name)            
            R = setfield(R, cor_field, n+1);
            R = setfield(R, rate_field, (n+1)/total_num);
        else
            R = setfield(R, rate_field, n/total_num);            
        end
    end
    
end
