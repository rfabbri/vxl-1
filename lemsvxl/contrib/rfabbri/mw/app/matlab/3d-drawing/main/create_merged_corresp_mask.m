%For each branch in the graph,
%Grab all the correspondence maps this curve has with all the
%curves contained so far in that branch
num_branches = size(curve_graph,1);
merged_corresp_mask_all = cell(num_branches,1);
merged_corresp_mask_cumulative = zeros(cur_size,1);
for cgbr=1:num_branches
    merged_corresp_mask = zeros(cur_size,1);
    for cgc=1:size(curve_graph_content{cgbr,1},1)
        corr_view = curve_graph_content{cgbr,1}(cgc,1);
        corr_curve = curve_graph_content{cgbr,1}(cgc,2);
        merged_corresp_mask = (merged_corresp_mask | query_corresp_masks{corr_view,1}(:,corr_curve));
        merged_corresp_mask_cumulative = (merged_corresp_mask_cumulative | query_corresp_masks{corr_view,1}(:,corr_curve));
    end
    merged_corresp_mask_all{cgbr,1} = merged_corresp_mask;
end