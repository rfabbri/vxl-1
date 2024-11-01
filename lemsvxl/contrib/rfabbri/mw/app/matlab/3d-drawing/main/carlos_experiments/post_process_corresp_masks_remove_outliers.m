%Post process the correspondence mask once again to remove
%outlier correspondences using neighborhood info
new_corresp_masks = corresp_masks;
%Post process the correspondence masks to fill the gaps
for vcl=1:numIM
    cur_view = corresp_masks{vcl,1};
    if(isempty(cur_view))
        continue;
    end
    for cl=1:size(cur_view,2)
        cur_mask = cur_view(:,cl);
        new_mask = cur_mask;
        for ss=3:numSamples-2
            % if(cur_mask(ss-1,1)==1 && cur_mask(ss+1,1)==1)
            % new_mask(ss,1) = 1;
            % elseif(cur_mask(ss-2,1)==1 && cur_mask(ss+2,1)==1)
            % new_mask(ss-1,1) = 1;
            % new_mask(ss,1) = 1;
            % new_mask(ss+1,1) = 1;
            % end
            if(ss==3 || ss==numSamples-2)
                mask_size = 2;
            elseif(ss==4 || ss==numSamples-3)
                mask_size = 3;
            elseif(ss==5 || ss==numSamples-4)
                mask_size = 4;
            else
                mask_size = 5;
            end
            prev_found = 0;
            post_found = 0;
            for sp=mask_size:-1:1
                if(cur_mask(ss-sp,1))
                    prev_mark = sp;
                    prev_found = 1;
                    break;
                end
            end
            for sa=mask_size:-1:1
                if(cur_mask(ss+sa,1))
                    post_mark = sa;
                    post_found = 1;
                    break;
                end
            end
            if(prev_found==1 && post_found==1)
                new_mask((ss-prev_mark):(ss+post_mark),1) = 0;
            end
        end
        new_corresp_masks{vcl,1}(:,cl) = new_mask;
    end
end
corresp_masks = new_corresp_masks;