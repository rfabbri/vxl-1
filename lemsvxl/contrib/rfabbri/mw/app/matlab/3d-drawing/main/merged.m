if(is_merged)
    %All new branches and their content is added
    curve_graph = [curve_graph; all_new_branches];
    lock_map = [lock_map; all_new_locks];
    new_content = [all_views(1,mrv)+1 cc];
    num_new_branches = size(all_new_branches,1);
    new_content_set = cell(num_new_branches,1);
    for nc=1:num_new_branches
        new_content_set{nc,1} = new_content;
    end
    curve_graph_content = [curve_graph_content; new_content_set];
else
    all_flags{all_views(1,mrv)+1,1}(curveID,1) = 0;
end