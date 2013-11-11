% This is /lemsvxl/contrib/firat/matlab/curve_matching_based_object_detection/write_hypothesis_file.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 22, 2011

function write_hypothesis_file(hypotheses, xgraph_file, out_file)
    fid = fopen(out_file, 'w');
    for i = 1:size(hypotheses, 1)
        fprintf(fid, '%s %f %f %f %f\n', xgraph_file, hypotheses(i,1), hypotheses(i,2), hypotheses(i,3), hypotheses(i,4));
    end
    fclose(fid);
end
