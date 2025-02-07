%Set the contents of the new branch
curve_graph_content = [curve_graph_content; cell(1,1)];
curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
%Set the correspondence flags for the new branch
merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
%Set the equivalence table for the new branch
equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
equiv_table_cur = [equiv_table_cur; cell(1,1)];
equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];