clear

dThresh = 1.5;          % inlier pixel distance threshold
gcThresh = .8;          % guided match correlation threshold
gdThresh = 1.5;

[file1, pathname1] = uigetfile({'*.jpg';'*.bmp';'*.tiff';'*.png'},'File Selector');
[file2, pathname2] = uigetfile({'*.jpg';'*.bmp';'*.tiff';'*.png'},'File Selector');


fprintf( 'Loading images...' );
tic 
img1 = imread(strcat(pathname1, file1));
if size(img1, 3) ~= 1
    img1 = rgb2gray(img1);
end;
img2 = imread(strcat(pathname2, file1));
if size(img2, 3) ~= 1
    img2 = rgb2gray(img2);
end;
fprintf( '%f\n', toc );

% load the SIFT frames and features
SIFT_file1 = strcat(file1, '_SIFTs.mat');
if exist(SIFT_file1) == 0
    fprintf( 'Calculating SIFTs for image 1...' )
    tic
    
    [loc,des,gss,dogss] = sift( img1, ...
                                    'firstoctave', 1, ...
                                    'verbosity', 0, ...
                                    'Threshold', 0.003, ...
                                    'NumLevels', 5, ...
                                    'sigman', 0.5) ;
                             
    fprintf( '%f\n', toc );
    save(SIFT_file1, 'loc', 'des');
    loc_1 = loc;
    des_1 = des;
    clear 'gss';
    clear 'dogss';
    clear 'loc';
    clear 'des';
else
    load(SIFT_file1);
    loc_1 = loc;
    des_1 = des;
    clear 'loc';
    clear 'des';    
end

SIFT_file2 = strcat(file2, '_SIFTs.mat');
if exist(SIFT_file2) == 0
    fprintf( 'Calculating SIFTs for image 2...' )
    tic
    
    [loc,des,gss,dogss] = sift( img2, ...
                                    'firstoctave', 1, ...
                                    'verbosity', 0, ...
                                    'Threshold', 0.003, ...
                                    'NumLevels', 5, ...
                                    'sigman', 0.5) ;
                             
    
    fprintf( '%f\n', toc );
    save(SIFT_file2, 'loc', 'des');
    loc_2 = loc;
    des_2 = des;
    clear 'gss';
    clear 'dogss';
    clear 'loc';
    clear 'des';
    
else
    load(SIFT_file2);
    loc_2 = loc;
    des_2 = des;
    clear 'loc';
    clear 'des';
end


fprintf('Calculating SIFT matches...');
tic

sift_matches = get_sift_matches(des_1, des_2, 0.8);
fprintf( '%f seconds\n', toc );    

if sift_matches == 0
    fprintf('there is NO matched SIFT feature found!\n');
    return;
end;

if size(sift_matches, 1) < 8
    fprintf('too few SIFT feature found!\n');
    return;    
end;


x = loc_1( [1:2], sift_matches(:,1))';
x(:,3) = 1;
xp = loc_2( [1:2], sift_matches(:,2))';
xp(:,3) = 1;


%run RANSAC
fprintf('-----RANSAC-----\n');
fprintf('Distance threshold: %f\n', dThresh );
tic;
e = .10;                % outlier probability              
p = .90;
s = 8;
N = log(1 - p)/log(1 - (1 - e)^s);
samplesTaken = 0;
bestResErr = Inf;
maxInliers = 0;
while samplesTaken < N
    %select samples
    samples = randperm(size(sift_matches,1));
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
            e = 1 - inlierCount / size( sift_matches,1 );
            if e > 0
                N = log(1 - p)/log(1 - (1 - e)^s);
            else
                N = 1;
            end              
        end
    end
    
    
    if mod(samplesTaken,10) == 0 
        fprintf( 'iterations:%d/%-4d  inliers:%d/%d  re:%f sampson:%f\n', ...
                  samplesTaken,floor(N),maxInliers,size(sift_matches,1), bestResErr,...
                  sampsonErrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
     
    end              
end

fprintf( 'it:%d  inliers:%d/%d  re:%f sampson:%f\n', ...
         samplesTaken,maxInliers,size(sift_matches,1), bestResErr,...
         sampsonErrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
fprintf('RANSAC took %f seconds.\n', toc);     

% now iteratively improve F
F = bestF;
matches = sift_matches(bestInliers,:);

x = loc_1( [1:2], matches(:,1))';
x(:,3) = 1;
xp = loc_2( [1:2], matches(:,2))';
xp(:,3) = 1;
F = getFnpt( F, x, xp );
bestF = F;
disp 'Done'   

% refine_threshold = 2;
% minimum_matches = 30;
% lastMatchCount = size(matches,1);
% matchCount = Inf;
% iterations = 1;
% while matchCount ~= lastMatchCount & matchCount >= minimum_matches;
%     % get F based on current matches
%     lastMatchCount = matchCount;
%     x = loc_1( [1:2], matches(:,1))';
%     x(:,3) = 1;
%     xp = loc_2( [1:2], matches(:,2))';
%     xp(:,3) = 1;    
%     F = getFnpt( F, x, xp );
%     fprintf( 'It:%d  Matches: %d  re:%f sampson:%f\n', iterations, size(matches,1),...
%              reserrF(F,x,xp), sampsonErrF(F,x,xp));
% 
%     % find new matches
%     % newmatches = guidedMatches( features1, features2, F, offs, gws, gcThresh, gdThresh );
%     A = x*F;
%     dist = zeros(size(matches, 1), 1);
%     for i=1:size(matches, 1)
%         dist(i) = dot(A(i,:), xp(i,:));
%     end;
%     average_dist = mean(dist);
%     std_dev = std(dist);
%     newmatches = matches(find(abs(dist-average_dist) < refine_threshold*std_dev),:);
%     
%     matchCount = size( newmatches, 1 );
%     matches = newmatches;
%     iterations = iterations + 1;   
%     fprintf('Average distance: %f. Max distance: %f. New matches: %d. std: %f\n', mean(dist), max(dist), matchCount, std_dev);
%     
% end
% 
% x = loc_1( [1:2], matches(:,1))';
% x(:,3) = 1;
% xp = loc_2( [1:2], matches(:,2))';
% xp(:,3) = 1;    
% F = getFnpt( F, x, xp );
% fprintf( 'It:%d  Matches: %d  re:%f sampson:%f\n', iterations, size(matches,1),...
%          reserrF(F,x,xp), sampsonErrF(F,x,xp));
% 
% bestF = F;     
% 
% disp 'Done'    