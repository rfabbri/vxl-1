% THIS IS JUST AN EXPERIMENTAL FILE THAT MODIFIES YONG'S CODE
% FOR BOTH FILE SPECIFICATION IN THE CODE, AS WELL AS FOCUSING ON SIFT
% COMPUTATION RATHER THAN F MATRIX COMPUTATION

clear

dThresh = 1.5;          % inlier pixel distance threshold
gcThresh = .8;          % guided match correlation threshold
gdThresh = 1.5;
masked = false;

%[file1, pathname1] = uigetfile({'*.jpg';'*.bmp';'*.tiff';'*.png'},'File Selector');
%[file2, pathname2] = uigetfile({'*.jpg';'*.bmp';'*.tiff';'*.png'},'File Selector');
%fname1 = strcat(pathname1, file1);
%fname2 = strcat(pathname2, file2);

%fname1='/home/rfabbri/Desktop/down/mazda-img1.png'
%fname2='/home/rfabbri/Desktop/down/mazda-img2.png'

%fname1='/home/rfabbri/work/sift-presents/banana-0.png'
%fname2='/home/rfabbri/work/sift-presents/banana-1.png'
fname1='/home/rfabbri/work/sift-presents/zebra-0-hda.png'
fname2='/home/rfabbri/work/sift-presents/zebra-1-hda.png'

%fname1='/home/rfabbri/lib/data/indian-bird-frames/sift-vs-curves/indian-bird-img1.png';
%fname2='/home/rfabbri/lib/data/indian-bird-frames/sift-vs-curves/indian-bird-img2.png';
%fname2='/home/rfabbri/lib/data/indian-bird-frames/indian-bird-img2_shortbline.png';

%fname1='/home/rfabbri/lib/data/pot/views/sherd1/set1/crop/crop-more/p1017376.jpg';
%fname2='/home/rfabbri/lib/data/pot/views/sherd1/set1/crop/crop-more/p1017377.jpg';

%fname1='/home/rfabbri/lib/data/candle/plastic-image-1-big.png'
%fname2='/home/rfabbri/lib/data/candle/plastic-image-2-big.png'

%fname1='/home/rfabbri/lib/data/plants-wire/focal-13dot4/p1010047-plant-r.png'
%fname2='/home/rfabbri/lib/data/plants-wire/focal-13dot4/p1010055-plant-r.png'

%fname1='/home/rfabbri/lib/data/airplane/landing-ss-1.png'
%fname2='/home/rfabbri/lib/data/airplane/landing-ss-1b.png'

%fname1='/home/rfabbri/lib/data/nonrigid/snakes/cobra-1.png'
%fname2='/home/rfabbri/lib/data/nonrigid/snakes/cobra-2.png'

%fname1='/home/rfabbri/lib/data/nonrigid/starfish/starfish-1.png'
%fname2='/home/rfabbri/lib/data/nonrigid/starfish/starfish-2.png'
%masked = true;
%fname1_mask='/home/rfabbri/lib/data/nonrigid/starfish/starfish-1-mask.png'
%fname2_mask='/home/rfabbri/lib/data/nonrigid/starfish/starfish-2-mask.png'

%fname1='/home/rfabbri/lib/data/nonrigid/liquid/milkdrop-1c.png'
%fname2='/home/rfabbri/lib/data/nonrigid/liquid/milkdrop-2c.png'

%fname1='/home/rfabbri/lib/data/snakepit/snakepit-11.png'
%fname2='/home/rfabbri/lib/data/snakepit/snakepit-12.png'

%fname1='/home/rfabbri/lib/data/boat/3boats-2a.png'
%fname2='/home/rfabbri/lib/data/boat/3boats-2b.png'

%fname1='/home/rfabbri/lib/data/boat/boat-4-fake.png'
%fname2='/home/rfabbri/lib/data/boat/boat-5-fake.png'

%fname1='/home/rfabbri/lib/data/boat/windsurf-1.png'
%fname2='/home/rfabbri/lib/data/boat/windsurf-2.png'

%fname1='/home/rfabbri/lib/data/nonrigid/liquid/milkdrop-1d.png'
%fname2='/home/rfabbri/lib/data/nonrigid/liquid/milkdrop-2d.png'

fprintf( 'Loading images...' );
tic 
img1 = rgb2gray(imread(fname1));
img2 = rgb2gray(imread(fname2));
if masked == true
  mask_1 = imread(fname1_mask); 
  mask_2 = imread(fname2_mask); 
end
fprintf( '%f\n', toc );

fprintf( 'Calculating SIFTs for image 1...' )
tic

[loc,des,gss,dogss] = sift( img1, ...
                                'firstoctave', 0, ...
                                'verbosity', 0, ...
                                'Threshold', 0.009, ...
                                'NumLevels', 5, ...
                                'sigman', 0.5) ;
                         
fprintf( '%f\n', toc );
loc_1 = loc;
des_1 = des;
clear 'gss';
clear 'dogss';
clear 'loc';
clear 'des';

fprintf( 'Calculating SIFTs for image 2...' )
tic

[loc,des,gss,dogss] = sift( img2, ...
                          'firstoctave', 0, ...
                          'verbosity', 0, ...
                          'Threshold', 0.009, ...
                          'NumLevels', 5, ...
                          'sigman', 0.5) ;
                   

fprintf( '%f\n', toc );
loc_2 = loc;
des_2 = des;
clear 'gss';
clear 'dogss';
clear 'loc';
clear 'des';
    
fprintf('Calculating SIFT matches...');
tic

if masked == true
  loc = loc_1;
  des = des_1;
  nsifts  = size(des_1,2);
  loc_out = zeros(size(loc));
  des_out = zeros(size(des));
  % remove any sifts outside mask
  iout = 1;
  for i=1:nsifts
    x1 = round(loc(1,i)); % todo: make sure coordinates are not inverted
    y1 = round(loc(2,i));
    
    if mask_1(y1,x1) ~= 0
      loc_out(:,iout) = loc(:,i);
      des_out(:,iout) = des(:,i);
      iout = iout + 1;
    end
  end
  loc_1 = loc_out(:,1:(iout-1));
  des_1 = des_out(:,1:(iout-1));
  clear loc_out des_out mask_1;

  loc = loc_2;
  des = des_2;
  nsifts  = size(des_2,2);
  loc_out = zeros(size(loc));
  des_out = zeros(size(des));
  % remove any sifts outside mask
  iout = 1;
  for i=1:nsifts
    x1 = round(loc(1,i)); % todo: make sure coordinates are not inverted
    y1 = round(loc(2,i));
    
    if mask_2(y1,x1) ~= 0
      loc_out(:,iout) = loc(:,i);
      des_out(:,iout) = des(:,i);
      iout = iout + 1;
    end
  end
  loc_2 = loc_out(:,1:(iout-1));
  des_2 = des_out(:,1:(iout-1));
  clear loc_out des_out mask_2;
end

% Function seems to have been written by Young. Why not use the one from UCLA?
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

x  = loc_1(1:2, sift_matches(:,1))';
x(:,3)  = 1;
xp = loc_2(1:2, sift_matches(:,2))';
xp(:,3) = 1;


%run RANSAC
fprintf('-----RANSAC-----\n');
fprintf('Distance threshold: %f\n', dThresh );
tic;
e = .50;                % outlier probability              
p = .99;
s = 8;
N = log(1 - p)/log(1 - (1 - e)^s);
N=6000;
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
            %resErr = sampsonerrf( F, x(inliers,1:2), xp(inliers,1:2) );
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
%            if e > 0
%                N = log(1 - p)/log(1 - (1 - e)^s);
%            else
%                N = 1;
%            end              
        end
    end
    
    
    if mod(samplesTaken,10) == 0 
        fprintf( 'iterations:%d/%-4d  inliers:%d/%d  re:%f sampson:%f\n', ...
                  samplesTaken,floor(N),maxInliers,size(sift_matches,1), bestResErr,...
                  sampsonerrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
     
    end              
end

fprintf( 'it:%d  inliers:%d/%d  re:%f sampson:%f\n', ...
         samplesTaken,maxInliers,size(sift_matches,1), bestResErr,...
         sampsonerrf( bestF, x(bestInliers,1:2), xp(bestInliers,1:2) ));
fprintf('RANSAC took %f seconds.\n', toc);     

% now iteratively improve F
F = bestF;

matches_ini = sift_matches;
x_ini = x;
xp_ini = xp;
F_ini = F; % F computed from best 8 pt sample

matches = sift_matches(bestInliers,:);
loc_1_inlier = loc_1(:,matches(:,1));
loc_2_inlier = loc_2(:,matches(:,2));
x = loc_1( [1:2], matches(:,1))';
x(:,3) = 1;
xp = loc_2( [1:2], matches(:,2))';
xp(:,3) = 1;
F = getFnpt( F, x, xp );
bestF = F; % F computed from inlier pts

SIFT_showmatches;
SIFT_showfeatures;
SIFT_showfeatures_inlier_outlier;
viewer_colines_rfabbri;
disp 'Done'    
