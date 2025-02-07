target_curve1 = [];
target_curve2 = [];
target_curve3 = [];
%Junction breaks the existing branch it's
%being attached to
if(arr_junction_indices_last(b,1) < arr_junction_indices(b,1))
    target_curve1 = target_curve(1:arr_junction_indices_last(b,1),:);
    target_curve2 = target_curve(arr_junction_indices_last(b,1):arr_junction_indices(b,1),:);
    target_curve3 = target_curve(arr_junction_indices(b,1):target_size,:);
else
    target_curve1 = target_curve(1:arr_junction_indices(b,1),:);
    target_curve2 = target_curve(arr_junction_indices(b,1):arr_junction_indices_last(b,1),:);
    target_curve3 = target_curve(arr_junction_indices_last(b,1):target_size,:);
end