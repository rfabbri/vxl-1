function [new_matches best_F] = RANSAC_Compute_F(loc_1, loc_2, init_matches);

dThresh = 1.0;          % inlier pixel distance threshold
gcThresh = 0.8;          % guided match correlation threshold
gdThresh = 1.5;

if init_matches == 0
    fprintf('there is NO matched SIFT feature found!\n');
    return;
end;

if size(init_matches, 1) < 8
    fprintf('too few SIFT feature found!\n');
    return;    
end;


x = loc_1( [1:2], init_matches(:,1))';
x(:,3) = 1;
xp = loc_2( [1:2], init_matches(:,2))';
xp(:,3) = 1;


%run RANSAC
fprintf('-----RANSAC-----\n');
fprintf('Distance threshold: %f\n', dThresh );
tic;
e = .98;                % outlier probability              
p = .99;
s = 8;
N = log(1 - p)/log(1 - (1 - e)^s);
samplesTaken = 0;
bestResErr = Inf;
maxInliers = 0;
while samplesTaken < N
    %select samples
    samples = randperm(size(init_matches,1));
    samples = samples(1:8);
    samplesTaken = samplesTaken + 1;
   
    %compute model    
    xs = x(samples,1:2); xsp = xp(samples,1:2);
    %F = getF7pt( x(samples,1:2), xp(samples,1:2) );
    F = getF8pt( x(samples,1:2), xp(samples,1:2) );
    for i=1:size(F,3)
        %determine inliers
        L1 = normalizeLine(F(:,:,i) * x');
        dist1 = abs(dot( xp', L1 ));
        
        L2 = normalizeLine(F(:,:,i)' * xp');
        dist2 = abs(dot( L2, x' ));
        
        inliers = find( dist1 < dThresh & dist2 < dThresh );
        inlierCount = size(inliers,2);            
        if inlierCount > 0
            resErr = sum( dist1(inliers).^2 + dist2(inliers).^2 ) / inlierCount;
            %resErr = sampsonErrf( F, x(inliers,1:2), xp(inliers,1:2) );
        else
            resErr = Inf;
        end
        if inlierCount > maxInliers | ...
           (inlierCount == maxInliers & resErr < bestResErr)
            % keep best found so far
            maxInliers = inlierCount;
            bestResErr = resErr;
            bestF = F(:,:,i);
            bestInliers = inliers;
            
            % adaptively update N
            e = 1 - inlierCount / size( init_matches,1 );
            if e > 0
                N = log(1 - p)/log(1 - (1 - e)^s);
            else
                N = 1;
            end              
        end
    end
    
    
    if mod(samplesTaken,10) == 0 
        fprintf( 'iterations:%d/%-4d  inliers:%d/%d  re:%f sampson:%f\n', ...
                  samplesTaken,floor(N),maxInliers,size(init_matches,1), bestResErr,...
                  sampsonErrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
     
    end              
end

fprintf( 'it:%d  inliers:%d/%d  re:%f sampson:%f\n', ...
         samplesTaken,maxInliers,size(init_matches,1), bestResErr,...
         sampsonErrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
fprintf('RANSAC took %f seconds.\n', toc);     

% now iteratively improve F
F = bestF;

new_matches = init_matches(bestInliers,:);

new_loc_1 = loc_1( [1:2], new_matches(:,1))';
new_loc_1(:,3) = 1;
new_loc_2 = loc_2( [1:2], new_matches(:,2))';
new_loc_2(:,3) = 1;
F = getFnpt( F, new_loc_1, new_loc_2 );
best_F = F;
