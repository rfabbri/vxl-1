function finalmask = tdseg(imfile, detfile, coeff, minscore)
        I = imread(imfile);
        load(detfile);
        num_det = length(all_detections);
        [M, N] = size(I);
        curr_bbox = [1 1 N M];
        old_assignment = ones(1,num_det) * -1;
        assignment = zeros(1,num_det);
        iterno = 0;
        %while any(old_assignment ~= assignment)                
                old_assignment = assignment;
                assignment = get_assignment(all_detections, num_det, curr_bbox, coeff, minscore);
                finalmask = zeros(M, N);
                for i = 1:num_det
                        j = assignment(i); 
                        if j
                                loc = all_detections{i}.Loc(j,:);
                                [h, w] = size(all_detections{i}.Mask);
                                dh = (h-1)/2;
                                dw = (w-1)/2;
                                y0 = loc(2) - dh;
                                y1 = loc(2) + dh; 
                                x0 = loc(1) - dw;
                                x1 = loc(1) + dw;
                                try
                                        finalmask(y0:y1, x0:x1) = finalmask(y0:y1, x0:x1) + all_detections{i}.Mask;                                                                                
                                catch
                                        continue
                                end
                        end
                end
                maxmask = max(max(finalmask));
                props1 = regionprops(double(finalmask > maxmask * 0.2), 'BoundingBox');
                tempbb = round(props1.BoundingBox);
                curr_bbox = [tempbb(1) tempbb(2) (tempbb(1)+tempbb(3)-1) (tempbb(2)+tempbb(4)-1)]; 
                iterno = iterno + 1;                               
        %end
        maxmask = max(max(finalmask));
        %I = double(I);
        finalmask(finalmask <= maxmask * 0.2) = 0; %0.2
        %I(finalmask == 0) = 0;
        %figure;imagesc(I); axis image; axis off;colormap gray 
        fprintf('Total number of iterations: %d\n',iterno);
end

function assignment = get_assignment(all_detections, num_det, curr_bbox, coeff, minscore)
        assignment = zeros(1, num_det);
        for i = 1:num_det
                detection = all_detections{i};
                num_choices = length(detection.Score);
                maxdet = 0;
                maxval = -Inf;
                for j = 1:num_choices
                        loc = detection.Loc(j,:);
                        NorLoc = (loc - curr_bbox(1:2)) ./ (curr_bbox(3:4) - curr_bbox(1:2));
                        if length(find(NorLoc < 0)) + length(find(NorLoc > 1)) == 0
                                locscore = 1 - sum((NorLoc - detection.ExpNorLoc).^2) / 2;
                                finalscore = coeff * detection.Score(j) + (1 - coeff) * locscore; 
                                if finalscore > maxval && finalscore > minscore
                                        maxval = finalscore;
                                        maxdet = j;
                                end 
                        end      
                end
                assignment(i) = maxdet;
        end
end

