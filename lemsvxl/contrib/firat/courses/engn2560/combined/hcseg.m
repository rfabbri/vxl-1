function fseg = hcseg(bu_out, td_out, ov_thres)
        load(bu_out);
        load(td_out);
        bu = segtree.bu;
        td = td > 0;
        N = max(bu(:));
        fseg = zeros(size(bu));
        for i = 1:N
                buregion = get_buregion(bu, salient_regions_table, i);
                buarea = sum(buregion(:));
                corres_tdregion = td;
                corres_tdregion(~buregion) = 0;
                tdarea = sum(sum(corres_tdregion));
                ratio = tdarea / buarea;
                if ratio > ov_thres
                      fseg(buregion) = 1;  
                end                
        end        
end

function buregion = get_buregion(bu, salient_regions_table, i)
        indices = find(salient_regions_table(i,:));
        buregion = ismember(bu, indices);
end
