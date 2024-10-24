keep_processing = 0;

% Check whether you need to keep processing
for av=1:numViews
    cur_flags = process_flags{all_views(1,av)+1,1};
    if(any(~cur_flags))
        keep_processing = 1;
        break;
    end
end