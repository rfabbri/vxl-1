function fseg = cseg(bu_out, td_out, ov_thres)
        bu = load(bu_out);
        load(td_out);
        td = td > 0;
        N = max(bu(:));
        fseg = zeros(size(bu));
        %figure; imagesc(td)
        %figure; imagesc(bu)
        for i = 1:N
                buregion = bu == i;
                %figure; imagesc(buregion)
                buarea = sum(buregion(:));
                corres_tdregion = td;
                corres_tdregion(~buregion) = 0;
                %figure; imagesc(corres_tdregion)
                tdarea = sum(sum(corres_tdregion));
                ratio = tdarea / buarea;
                if ratio > ov_thres
                      fseg(buregion) = 1;  
                end
                %pause
        end
        %figure;imagesc(fseg)
end
