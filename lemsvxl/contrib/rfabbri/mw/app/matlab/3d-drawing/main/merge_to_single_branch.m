only_merge_to_single_branch = 0;
%Check to see if we are close to an existing
%junction, avoid merging this sample with
%multiple branches if that's the case
for jn=1:size(all_junctions,1)
    if(norm(queryCurve(ms,:) - all_junctions(jn,:)) < junction_dist_threshold)
        only_merge_to_single_branch=1;
        break;
    end                                     
end
%only_merge_to_single_branch = 0;
if(~only_merge_to_single_branch)
    %Go over each branch with correspondence, then
    %store the IDs of the 'close enough' branches
    for br=1:num_branches
        if(arr_branch_dist(br,1)>-1 && arr_branch_dist(br,1)<dist_threshold && br~=closest_branch)
            arr_branch_id_cur = [arr_branch_id_cur br];
        end
    end
end