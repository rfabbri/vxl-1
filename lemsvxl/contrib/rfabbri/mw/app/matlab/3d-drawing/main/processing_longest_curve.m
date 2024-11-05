%this is responsible for finding the longest curve in a cluster,
%breaking it into pieces at discontinuities, and 
%initializing a curve graph with these pieces. 

%First we find the longest curve in the cluster
longest_curve_size = 0;
longest_curve_view = -1;
longest_curve_id = -1;

for lcv=1:numViews
    cur_clusters = clusters{all_views(1,lcv)+1,1};
    if(isempty(cur_clusters))
        continue;
    end
    num_curves_in_cluster = size(cur_clusters,2);
    for cc=1:num_curves_in_cluster
        curveID = cur_clusters(1,cc);
        queryCurve = all_recs_iter{all_views(1,lcv)+1,1}{1,curveID};
        cur_size = size(queryCurve,1);
        if(cur_size > longest_curve_size)
            longest_curve_view = all_views(1,lcv)+1;
            longest_curve_view_index = lcv;
            longest_curve_id = curveID;
            longest_curve_cluster_id = cc;
            longest_curve_size = cur_size;
        end
    end
end
        
%We process the longest curve to break it in discontinuities
longest_curve_pieces = [];
longest_curve = all_recs_iter{longest_curve_view,1}{1,longest_curve_id};
start_point = 1;
       
for lcs=2:size(longest_curve,1)
    cur_dist = norm(longest_curve(lcs,:)-longest_curve(lcs-1,:));
    if(cur_dist>sample_break_threshold)
        current_piece = longest_curve(start_point:lcs-1,:);
        %Be more lenient here with length threshold
        if(4*get_length(current_piece)>branch_length_threshold)
            longest_curve_pieces = [longest_curve_pieces; cell(1,1)];
            lcp_size = size(longest_curve_pieces,1);
            longest_curve_pieces{lcp_size,1} = current_piece;
        end
        start_point=lcs;
    end
    if(lcs==size(longest_curve,1))
        current_piece = longest_curve(start_point:lcs-1,:);
        %Be more lenient here with length threshold
        if(4*get_length(current_piece)>branch_length_threshold)
            longest_curve_pieces = [longest_curve_pieces; cell(1,1)];
            lcp_size = size(longest_curve_pieces,1);
            longest_curve_pieces{lcp_size,1} = current_piece;
        end
    end
end
       
%We initialize the curve graph with these longest curve pieces
curve_graph = longest_curve_pieces;
lcp_size = size(longest_curve_pieces,1);
%curve_graph{1,1} = all_recs_iter{longest_curve_view,1}{1,longest_curve_id};
curve_graph_content = cell(lcp_size,1);
for lcp=1:lcp_size
    curve_graph_content{lcp,1} = [longest_curve_view longest_curve_cluster_id];
end
%A pair of flags per branch, marking whether endpoints are locked with a junction
lock_map = zeros(lcp_size,2);
        
merge_flags = cell(numViews,1);
for mfv=1:numViews
    merge_flags{mfv,1} = zeros(size(clusters{all_views(1,mfv)+1,1}));
end
merge_flags{longest_curve_view_index,1}(1,longest_curve_cluster_id) = 1;