%K: smoothing iterations
%N: number of points
function sampled_curve_coords = sample_curve_old(curve_coords, N, is_closed)
    if is_closed
        segment_lengths = sqrt(sum((curve_coords(1:end,:) - curve_coords([2:end 1], :)).^2,2));
    else
        segment_lengths = sqrt(sum((curve_coords(1:end-1,:) - curve_coords(2:end, :)).^2,2));
    end
    zero_indices = find(segment_lengths == 0);
    segment_lengths(zero_indices) = [];
    curve_coords(zero_indices,:)= [];
    s = [0; cumsum(segment_lengths)];
    sampled_curve_coords = zeros(N, 2);    
    if is_closed  
        l = s(end)/N;      
        sampled_curve_coords(:,1) = interp1(s, curve_coords([1:end 1],1), 0:l:s(end)-l);
        sampled_curve_coords(:,2) = interp1(s, curve_coords([1:end 1],2), 0:l:s(end)-l);
    else
        l = s(end)/(N-1);
        sampled_curve_coords(:,1) = interp1(s, curve_coords(:,1), 0:l:s(end));
        sampled_curve_coords(:,2) = interp1(s, curve_coords(:,2), 0:l:s(end));
    end        
    V = zeros(1,N-1);
    for i = 1:N-1
        V(i) = sqrt((sampled_curve_coords(i+1,1)-sampled_curve_coords(i,1))^2 + (sampled_curve_coords(i+1,2)-sampled_curve_coords(i,2))^2);
    end
    if std(V,1) > 1e-10
        sampled_curve_coords = sample_curve(sampled_curve_coords, N, is_closed);
    end
end
