%{
Code to reconstruct a Medical Image by fixing points using the following methods
1)Maxima and Minima
2)Max, Min, Watershed, Watercourse
3)Max, Min, Saddles
4)Edge Points
5)Max, Min, Edges
The Indices of respective points are retained and their intensities are fixed as that in the
original image or the blurred image.
The images are reconstructed by solving the Heat Equation.
The Convergence criteria is that the maximum difference between successive reconstructions be less
than 1e-4, though the user can change it as per their requirements. 
Variable to calculate compresssion ratios are also calculated.
%}

%%Take convergence 1e-2 for MI1. 

img=imread('E:\USA Trip\Work\Original Images\MI1.png');
img=rgb2gray(img);

% f=fspecial('average',[3 3]);          %Averaging Filter
f=[0 1/4 0;1/4 0 1/4;0 1/4 0];          %Laplacian Filter

subplot(321);
imshow(img);
title('Original Image');

%%Reducing number of critical points using connected components 
%%Image must be of equal dimensions for it work properly
maxima=imregionalmax(img);
minima=imregionalmin(img);
ccmax=bwconncomp(maxima);
ccmin=bwconncomp(minima);
smax=regionprops(ccmax,'centroid');
smin=regionprops(ccmin,'centroid');
centmax=cat(1,smax.Centroid);
centmin=cat(1,smin.Centroid);
centmax=round(centmax);
centmin=round(centmin);
[a,~]=size(centmax);
maxima=zeros(size(img));
for i=1:a
    maxima(centmax(i,1),centmax(i,2))=1;
end
[a,~]=size(centmin);
minima=zeros(size(img));
for i=1:a
    minima(centmin(i,1),centmin(i,2))=1;
end

imgd=double(img);
ws=watershed(imgd);                         %Compute Watershed Matrix
wc=watershed(-imgd);                        %Compute Watercourse Matrix
[a,b]=size(img);

%Fixing Maxima and Minima
I=zeros(a,b);
for i=1:a
    for j=1:b
        if ( maxima(i,j)==1 || minima(i,j)==1 )
            I(i,j)=img(i,j);                         %Fixing Points of either maxima or minima
        end
    end
end

n=1;
Pre=I;
while 1
   MaxMin=imfilter(Pre,f);
   for i=1:a
       for j=1:b
           if ( I(i,j)~=0 )
               MaxMin(i,j)=I(i,j);
           end
       end
   end
   n=n+1;
   if ( n>2 && max(max(MaxMin-Pre))<1e-2)
      break
   end
   Pre=MaxMin;
end 

subplot(322);
imshow(MaxMin,[]);
title('Reconstructed Image Maxima and Minima');

%Fixing Maxima Minima Watershed and Waterscourse
I=zeros(a,b);
for i=1:a
    for j=1:b
        if (maxima(i,j)==1 || minima(i,j)==1 || ws(i,j)==0 || wc(i,j)==0 )
            I(i,j)=img(i,j);
        end
    end
end

n=1;
Pre=I;
while 1
    MaxMinWcWs=imfilter(Pre,f);
    for i=1:a
        for j=1:b
            if ( I(i,j)~=0 )
                MaxMinWcWs(i,j)=img(i,j);
            
            end
        end
    end
    n=n+1;
    if ( n>2 && max(max(MaxMinWcWs-Pre))<1e-2)
        break
    end
    Pre=MaxMinWcWs;
end

subplot(323);
imshow(MaxMinWcWs,[]);
title('Using Maxima Minima Ws Wc');

%Fixing Max Min Saddles
I=zeros(a,b);
sdd=zeros(a,b);                     %Saddle Matrix
for i=2:a-1
    for j=2:b-1
        nbh=wc(i-1:i+1,j-1:j+1);
        if ( ws(i,j)==0 && ismember(0,nbh) )
            sdd(i,j)=1;                         %Saddle Points are taken at intersections of watershed and watercourse lines
        end           
    end
end

for i=1:a
    for j=1:b
        if ( maxima(i,j)==1 || minima(i,j)==1 || sdd(i,j)==1 )
            I(i,j)=img(i,j);
        end
    end
end
  
n=1;
Pre=I;
while 1
    MaxMinSaddle=imfilter(Pre,f);
    for i=1:a
        for j=1:b
            if ( I(i,j)~=0 )
                MaxMinSaddle(i,j)=img(i,j);
            end
        end
    end
    n=n+1;
    if( n>2 && max(max(MaxMinSaddle-Pre))<1e-2)
        break
    end
    Pre=MaxMinSaddle;
end

subplot(324);
imshow(MaxMinSaddle,[]);
title('Reconstructed using Max Min Saddle');

%Fixing only Edges
I=zeros(a,b);
ed=edge(img);       %Edges are detected using default sobel edge detector
for i=1:a
    for j=1:b
        if(ed(i,j)==1)
            I(i,j)=img(i,j);
        end
    end
end

n=1;
Pre=I;
while 1
    Rec=imfilter(Pre,f);
    for i=1:a
        for j=1:b
            if (I(i,j)~=0)
                Rec(i,j)=I(i,j);
            end
        end
    end
    n=n+1;
    if( n>2000)
        break
    end
    Pre=Rec;
end

subplot(325);
imshow(Rec,[]);
title('Reconstruction using Edges');


%Fixing Maxima Minima and Edges
I=zeros(a,b);
ed=edge(img);
for i=1:a
    for j=1:b
        if(maxima(i,j)==1 || minima(i,j)==1 || ed(i,j)==1)
            I(i,j)=img(i,j);
        end
    end
end

n=1;
Pre=I;
while 1
    Rec=imfilter(Pre,f);
    for i=1:a
        for j=1:b
            if (I(i,j)~=0)
                Rec(i,j)=I(i,j);
            end
        end
    end
    n=n+1;
    if( n>2000)
        break
    end
    Pre=Rec;
end

subplot(326);
imshow(Rec,[]);
title('Reconstruction using Maxima Minima Edges');

%These variable can be used to calculate the compression ratios

c1=length(find(maxima==1));     %Number of points fixed as Maxima
c2=length(find(minima==1));     %Number of points fixed as Minima       
c3=length(find(ws==0));         %Number of points fixed as Watershed
c4=length(find(wc==0));         %Number of points fixed as Watercourse
c5=length(find(sdd==1));        %Number of points fixed as Saddles
c6=a*b;                         %Number of Points in Image            
