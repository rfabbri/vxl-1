% This is match_model_query_single_scale.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 25, 2011

function [match_center, match_length, match_reverse, match_scores] = match_model_query_single_scale(CM, CQ, consider_reverse, N, L, K, CM_desc, CM_rev_desc, CQ_desc)
    if mod(N,2) == 0
        N = N+1;
    end
    if mod(L,2) == 0
        L = L+1;
    end
    if ~exist('CM_desc','var')        
        CM_desc = curve2desc(CM, K, N, 1);
        if consider_reverse
            CM_rev_desc = curve2desc(CM(end:-1:1,:), K, N, 1);
        end
    end  
    if ~exist('CQ_desc','var')  
        CQ_desc = curve2desc(CQ, K, L, 0);
    end
    [CQ_desc_h, CQ_desc_w] = size(CQ_desc);
    delta_h = (CQ_desc_h - 1) / 2;
    delta_w = (CQ_desc_w - 1) / 2;        
    
    wrapped_CM_desc = [CM_desc(:,end-delta_w+1:end) , CM_desc, CM_desc(:, 1:delta_w)];
    wrapped_CM_desc = [wrapped_CM_desc(end-delta_h+1:end,:) ; wrapped_CM_desc ; wrapped_CM_desc(1:delta_h,:)];  
    [R1, c1] = diagonal_normalized_cross_correlation(CQ_desc, wrapped_CM_desc);
    center1 = mod(c1-delta_w-1,  N) + 1;      
    
    if consider_reverse
        wrapped_rev_CM_desc = [CM_rev_desc(:,end-delta_w+1:end) , CM_rev_desc, CM_rev_desc(:, 1:delta_w)];
        wrapped_rev_CM_desc = [wrapped_rev_CM_desc(end-delta_h+1:end,:) ; wrapped_rev_CM_desc ; wrapped_rev_CM_desc(1:delta_h,:)]; 
        [R2, c2] = diagonal_normalized_cross_correlation(CQ_desc, wrapped_rev_CM_desc);           
        center2 = N - mod(c2-delta_w-1,  N);
    else
        R2 = []; center2 = [];
    end   
    
    match_scores = [R1; R2];           
    match_center = [center1; center2];
    match_reverse = [zeros(size(center1)); ones(size(center2))];
    match_length = ones(length(match_center),1) * L;
end

