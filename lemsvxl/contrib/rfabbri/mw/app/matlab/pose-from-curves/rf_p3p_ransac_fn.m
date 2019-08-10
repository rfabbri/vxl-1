function [bestRot,bestTransl,bestResErr,bestResErrVec,vsolve_time]=rf_p3p_ransac_fn(...
ids1, gama_pert, Gama_all, K_gt, gama_pert_img, dThresh)

if nargin < 6
  dThresh = 1.5; % inlier distance threshold
end

e = .50; % outlier probability              
p = .99; % probability we hit all inliers sample
s = 2;   % sample size
% number of runs:
N = log(1 - p)/log(1 - (1 - e)^s);
% harcode:
%N=1200; 
N=1000; 
%N=3; 


samplesTaken = 0;
bestResErr = Inf;
maxInliers = 0;
num_corresps = size(ids1,1);
vsolve_time=zeros(1,N);
while samplesTaken < N
    %select samples

    corr_id1 = randint(1,1,[1 num_corresps]);
    corr_id2 = randint(1,1,[1 num_corresps]);
    while corr_id2 == corr_id1
      corr_id2 = randint(1,1,[1 num_corresps]);
    end
    corr_id3 = randint(1,1,[1 num_corresps]);
    while corr_id3 == corr_id1 | corr_id3 == corr_id2
      corr_id3 = randint(1,1,[1 num_corresps]);
    end

    samplesTaken = samplesTaken + 1;

    id1 = ids1(corr_id1);
    id2 = ids1(corr_id2);
    id3 = ids1(corr_id3);
   
    %compute model    
    idtriplet =  [id1 id2 id3];
    [Rots,Transls,solve_time] = rf_p3p_root_find_function_any(gama_pert(idtriplet, 1:2),Gama_all(idtriplet,:))
    vsolve_time(samples_taken) = solve_time;

    num_sols = length(Rots);
    for i=1:num_sols
        % determine inliers
        errors = rf_reprojection_error(K_gt*[Rots{i} Transls{i}],...
                 gama_pert_img(ids1,:), Gama_all(ids1,:));

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
            bestResErrVec = errors(inliers);
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
