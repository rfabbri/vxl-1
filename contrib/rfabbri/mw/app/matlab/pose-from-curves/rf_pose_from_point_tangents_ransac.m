%function [R,T]=rf_pose_from_point_tangents_ransac()

dThresh = 1.5 % inlier distance threshold

e = .50; % outlier probability              
p = .99; % probability we hit all inliers sample
s = 2;   % sample size
% number of runs:
N = log(1 - p)/log(1 - (1 - e)^2);
% harcode:
N=100; 


samplesTaken = 0;
bestResErr = Inf;
maxInliers = 0;
num_corresps = size(ids1,1);
while samplesTaken < N
    %select samples

    corr_id1 = randint(1,1,[1 num_corresps]);
    corr_id2 = randint(1,1,[1 num_corresps]);
    while corr_id2 == corr_id1
      corr_id2 = randint(1,1,[1 num_corresps]);
    end

    samplesTaken = samplesTaken + 1;

    id1 = ids1(corr_id1);
    id2 = ids1(corr_id2);
   
    %compute model    
    [Rots, Transls, degen] = rf_pose_from_point_tangents_root_find_function_any(...
    gama_pert(id1,:)', tgt_pert(id1,:)', gama_pert(id2,:)', tgt_pert(id2,:)',...
    Gama_all(id1,:)', Tgt_all(id1,:)', Gama_all(id2,:)', Tgt_all(id2,:)');

    num_sols = length(Rots);
    for i=1:num_sols
        %determine inliers
        errors = rf_reprojection_error(K_gt*[Rots{i} Transls{i}],...
                 gama_all_img(ids1,:), Gama_all(ids1,:));

        inliers = find( errors < dThresh );
        inlierCount = length(inliers);            
        if inlierCount > 0
            resErr = mean(errors(inliers));
        else
            resErr = Inf;
        end
        if inlierCount > maxInliers | ...
           (inlierCount == maxInliers & resErr < bestResErr)
            % keep best found so far
            maxInliers = inlierCount;
            bestResErr = resErr;
            bestRot = Rots{i};
            bestTransl = Transls{i};
            bestInliers = inliers;
        end
    end
    
    if mod(samplesTaken,10) == 0 
        fprintf( 'iterations:%d/%-4d  inliers:%d/%d  err:%f\n', ...
                  samplesTaken,floor(N),maxInliers,num_corresps, bestResErr);
    end              
end
