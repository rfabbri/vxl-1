%This code aims to plot the maxima, minima, saddles, watershed and watersourse lines in an image

%%Random Matrix
% I=zeros(256,256);
% I(15:45,211:241)=255;
% I(101:134,55:86)=255;
% I(194:223,144:177)=255;

%%Peaks Function
% I=peaks(50);
% I(15:20,14:19)=0;
% I(21:27,22:30)=0;
% I(28:34,31:37)=0;

I=imread('E:\USA Trip\Work\Original Images\Cameraman.tif');
%I=rgb2gray(I);
%I=imcrop(I,[100 65 60 60]);   %Cropped Region for Cheetah Image
%I=imcrop(I,[250 120 100 100]);   %Cropped Region for Lena Image

I=double(I);
%Compute Minima and Watershed
BW = imregionalmin(I);
length(find(BW==true));
[rmin, cmin] = find(BW);      %Storing locations of minima pixels  
WS = watershed(I);
[rws, cws] = find(WS==0);     %Storing locations of watershed pixels

%Compute Maxima and Watercourse
BW = imregionalmax(I);
length(find(BW==true)); 
[rmax, cmax] = find(BW);       %Storing locations of maxima pixels
WC = watershed(-I);
[rwc, cwc] = find(WC==0);      %Storing locations of watercourse pixels

%Compute Saddle Points
[a, b]=size(I);
sdd=zeros(a,b);
nbh=[];
for i=2:a-1
    for j=2:b-1
%         nbh=WC(i-1:i+1,j-1:j+1);
        if ( WS(i,j)==0 && WC(i,j)==0) 
            sdd(i,j)=1;             %Saddle Point is taken at intersection of watershed and watercourse lines.
        end           
    end
end

%%Reducing number of saddle points using connected components 
ccmax=bwconncomp(sdd);
smax=regionprops(ccmax,'centroid');
centmax=cat(1,smax.Centroid);
centmax=round(centmax);
[a,b]=size(centmax);
sdd=zeros(size(I));
for i=1:a
    sdd(centmax(i,1),centmax(i,2))=1;
end
[rs, cs]=find(sdd==1);              %Storing indices of saddle points


%Plotting Minima and Watershed lines
figure;
imagesc(I); colormap gray; hold on; 
plot(cmin, rmin, 'o', 'MarkerSize', 2, 'MarkerEdgeColor','b', 'MarkerFaceColor','b','LineWidth', 1);	 
plot(cwc, rwc, '+', 'MarkerSize', 2, 'MarkerEdgeColor','b', 'LineWidth', 1);  
title('Minima and Watercourse');
axis equal;
%Plotting Maxima and Watercourse Lines
figure;
imagesc(I); colormap gray; hold on; 
plot(cmax, rmax, 'o', 'MarkerSize', 2, 'MarkerEdgeColor','r', 'MarkerFaceColor','r','LineWidth', 1);
plot(cws, rws, '+', 'MarkerSize', 2, 'MarkerEdgeColor','r', 'LineWidth', 1);   
title('Maxima and Watershed');	     
axis equal;
%Plotting Saddle Points
figure;
imagesc(I); colormap gray; hold on;
plot(cs,rs,'d','MarkerSize',2,'Color','m');
title('Saddle Points');
axis equal;