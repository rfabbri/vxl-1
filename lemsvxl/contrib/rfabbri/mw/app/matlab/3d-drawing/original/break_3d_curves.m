clear all;
close all;

read_curve_sketch4;

ncurves = size(recs,2);
broken_recs = cell(1,5*ncurves);
brokenIND = 1;

for c=1:ncurves
   
    numsamples = size(recs{c},1);
    startSample = 1;
    
    for s=2:numsamples
        distVec = [recs{c}(s,1)-recs{c}(s-1,1) recs{c}(s,2)-recs{c}(s-1,2) recs{c}(s,3)-recs{c}(s-1,3)];
        dist = norm(distVec);
        if(dist > 0.05)
            if(s>startSample+1)
                broken_recs{1,brokenIND} = recs{c}(startSample:(s-1),:);
                brokenIND = brokenIND+1;
            end
            startSample = s;
        end
    end
    broken_recs{1,brokenIND} = recs{c}(startSample:numsamples,:);
    brokenIND = brokenIND+1;
    
end