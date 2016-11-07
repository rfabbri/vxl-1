%given a data stream, determine the appropriate time scale.
function [time_scale, time_scale_change_indx] = split_time_scale(data_stream)

% basic split and merge algorithm
% this algorithm will determine time scale by looking at the time
% difference between two different events.

% axioms:
%   1. Two events cannot occur at the same times.
%   2. Do not use a finer scale than necessary
% This function will consider splits only, not merges.
time_scale = [];
time_scale_change_indx = [];
curr_time_scale = nan;
for trel = 1:length(data_stream)
    
    if((data_stream(trel) == 1) && trel == 1)
        curr_event = trel;
    end
    
    if((data_stream(trel) == 1) && trel > 1 )
        prev_event = curr_event;
        curr_event = trel;
        prev_time_scale = curr_time_scale;
        curr_time_scale = curr_event - prev_event;
        
        if (prev_time_scale ~= curr_time_scale)
            time_scale_change_indx = [time_scale_change_indx trel];
            time_scale = [time_scale curr_time_scale];
        end
        
    end
    
end