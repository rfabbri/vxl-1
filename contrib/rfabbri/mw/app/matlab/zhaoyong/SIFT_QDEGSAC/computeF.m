%clear

%file1 = 'tcorner1.jpg';
%file2 = 'tcorner2.jpg';
if not(exist('file1'))
	file1 = 'apt2.jpg';
	file2 = 'apt1.jpg';
	fws  = 15;             % feature window size
	fws2 = 25;             % feature window size
	pws  = 100;
	gws  = 100;            % guided window size
	offs = [ 0  0]; 
	pThresh = .8;           % putative match correlation threshold
	dThresh = 1.5;          % inlier pixel distance threshold
	gcThresh = .8;          % guided match correlation threshold
	gdThresh = 1.5;
end
    
    
%if not(exist('img1'))
fprintf( 'Loading images...' );
tic 
img1 = imread(file1);
img2 = imread(file2);
fprintf( '%f\n', toc );
%end

% load the corners
cornerfile1 = strcat(file1, '_corners.txt');
f = fopen( cornerfile1, 'r' );
if f == -1
    fprintf( 'Calculating corners for image 1...' )
    tic
    corners1 = corner_detector(img1);
    fprintf( '%f\n', toc );
    f = fopen( cornerfile1, 'w+' );
    fprintf( f, '%f %f\n', corners1' );
    fclose(f);
else
    corners1 = fscanf(f, '%f %f\n', [2 Inf] )';
    fclose(f);
end

cornerfile2 = strcat(file2, '_corners.txt');
f = fopen( cornerfile2, 'r' );
if f == -1
    fprintf( 'Calculating corners for image 2...' )
    tic
    corners2 = corner_detector(img2);
    fprintf( '%f\n', toc );
    f = fopen( cornerfile2, 'w+' );
    fprintf( f, '%f %f\n', corners2' );
    fclose(f)
else
    corners2 = fscanf(f, '%f %f\n', [2 Inf] )';
    fclose(f);
end


fprintf( 'Calculating features (fws=%d)...', fws )
tic
features1 = makeFeatures( corners1, img1, fws );
features2 = makeFeatures( corners2, img2, fws );
fprintf( '%f\n', toc ); 



if not(exist('pmatches'))
    fprintf('Correlation threshold: %f\n', pThresh );    
    fprintf('Calculating putative matches...');
    tic
    features1.matches = getWindowedMatches( features1, features2, offs, pws, pThresh );
    features2.matches = getWindowedMatches( features2, features1, -offs, pws, pThresh );
    pmatches = bestMutualMatches( features1, features2 );
    fprintf( '%f\n', toc );    
end



x = features1.pos( pmatches(:,1), [2 1] );
x(:,3) = 1;
xp = features2.pos( pmatches(:,2), [2 1] );
xp(:,3) = 1;



%run RANSAC
fprintf('-----RANSAC-----\n');
fprintf('Distance threshold: %f\n', dThresh );
if not(exist('bestF'))
e = .60;                % outlier probability              
p = .99;
s = 8;
N = log(1 - p)/log(1 - (1 - e)^s);
samplesTaken = 0;
bestResErr = Inf;
maxInliers = 0;
while samplesTaken < N
    %select samples
    samples = randperm(size(pmatches,1));
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
            e = 1 - inlierCount / size( pmatches,1 );
            if e > 0
                N = log(1 - p)/log(1 - (1 - e)^s);
            else
                N = 1;
            end              
        end
    end
    
    
    if mod(samplesTaken,10) == 0 
        fprintf( 'iterations:%d/%-4d  inliers:%d/%d  re:%f sampson:%f\n', ...
                  samplesTaken,floor(N),maxInliers,size(pmatches,1), bestResErr,...
                  sampsonErrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
     
    end              
end    
end    
fprintf( 'it:%d  inliers:%d/%d  re:%f sampson:%f\n', ...
         samplesTaken,maxInliers,size(pmatches,1), bestResErr,...
         sampsonErrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
     

fprintf('-----Iterative Improvement-----\n');     
fprintf('Correlation threshold: %f\n', gcThresh );
fprintf('Distance threshold: %f\n', gdThresh );
fprintf( 'Calculating features  (fws=%d)...', fws2 )
tic
features1 = makeFeatures( corners1, img1, fws2 );
features2 = makeFeatures( corners2, img2, fws2 );
fprintf( '%f\n', toc );
     
% now iteratively improve F
F = bestF;
matches = pmatches(bestInliers,:);
lastMatchCount = size(matches,1);
matchCount = Inf;
iterations = 1;
while matchCount ~= lastMatchCount 
    % get F based on current matches
    lastMatchCount = matchCount;
    x = features1.pos( matches(:,1), [2 1] );
    xp = features2.pos( matches(:,2), [2 1] );
    F = getFnpt( F, x, xp );
    fprintf( 'It:%d  Matches: %d  re:%f sampson:%f\n', iterations, size(matches,1),...
             reserrF(F,x,xp), sampsonErrF(F,x,xp));

    % find new matches
    %fprintf( 'Calculating guided matches...' )
    tic
 	%features1.matches = guidedMatches( features1, features2, F, offs, gws, gcThresh, gdThresh );
 	%features2.matches = guidedMatches( features2, features1, F, -offs, gws, gcThresh, gdThresh );
 	%newmatches = bestMutualMatches( features1, features2 );
    newmatches = guidedMatches( features1, features2, F, offs, gws, gcThresh, gdThresh );
    matchCount = size( newmatches, 1 );
    matches = newmatches;
    iterations = iterations + 1;
    %fprintf( '%f\n', toc );
   
end    
     
disp 'Done'
    
    