%First gather the corresponding point data
vote_colors(1,:) = [1 0 1];
vote_colors(2,:) = [0 1 0];
vote_colors(3,:) = [0 0 1];
vote_colors(4,:) = [1 1 0];
vote_colors(5,:) = [0 1 1];

for ss=1:size(queryAlignment_dense,1)
    cur_alignment = queryAlignment_dense{ss,1};
    for a=1:size(cur_alignment,1)
        cur_corr = cur_alignment(a,:);
    
        corr_view = cur_corr(1,1);
        corr_curve = cur_corr(1,2);
        corr_sample = cur_corr(1,3);
        corr_votes = cur_corr(1,4);

        corr_curve_3d = all_recs{corr_view,1}{1,corr_curve};

        %Find the index in the clusters
        clust_ind = find(clusters{corr_view,1}==corr_curve);

        if(size(clust_ind,1)>1)
            disp('ERROR: Cluster vectors contain duplicate IDs!!');
        end

        if(isempty(clust_ind))
            %disp('Cluster candidate with insufficient votes is being ignored');
            continue;
        end

        corr_native{corr_view,1}{1,clust_ind} = [corr_native{corr_view,1}{1,clust_ind}; corr_curve_3d(corr_sample,:)];
        corr_other{corr_view,1}{1,clust_ind} = [corr_other{corr_view,1}{1,clust_ind}; queryCurve(ss,:)];

        if(corr_votes>4)
            color_index = 5;
            elseif(corr_votes>3)
                color_index = 4;
            elseif(corr_votes>2)
                color_index = 3;
            elseif(corr_votes>1)
                color_index = 2;
            else
                color_index = 1;
        end

        corr_colors{corr_view,1}{1,clust_ind} = [corr_colors{corr_view,1}{1,clust_ind}; vote_colors(color_index,:)];
    end
end