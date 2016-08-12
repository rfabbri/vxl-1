%{
This Code takes as an input any 2-D matrix and returns the Reconstructed Image by fixing  
1)Maxima and Minima using Connected Components
2)Maxima , Minima , Watershed and Watercourse Lines
3)Maxima , Minima and Saddle Points using Connected Components
4)Edge Points
5)Maxima , Minima and Edge Points
Reconstruction is done using the Heat Equation. It also returns 7 variables which
can be used for calculating the compression ratio for each of the reconstructed images.           
%}

function [c1,c2,c3,c4,c5,c6,c7,MaxMin,MaxMinWcWs,MaxMinSaddle,Ed,MaxMinEd] = MeanRec(Img)

img=Img;
%Converting the Image from rgb to grayscale if required
if (size(img,3)==3)
   img=rgb2gray(img);
end
%Create a 3x3 averaging filter
f=fspecial('average',[3 3]);

subplot(321);
imshow(img);
title('Original Image');

%Calculate maxima,minima,watershed and watercourse
imgd=double(img);
ws=watershed(imgd);
wc=watershed(-imgd);
%%This is for connected components for maxima and minima points
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

%Calculate size of the image
[a,b]=size(img);

%Fixing Maxima and Minima
I=zeros(a,b);
for i=1:a
    for j=1:b
        if ( maxima(i,j)==1 || minima(i,j)==1 )
            I(i,j)=img(i,j);%Fixing Points where either maxima or minima is 1,ie, either or maxima or minima
        end
    end
end

n=1;
Pre=I;
while 1
   MaxMin=imfilter(Pre,f);%Applying Filter to image
   for i=1:a
       for j=1:b
           if ( I(i,j)~=0 )
               MaxMin(i,j)=I(i,j); %Retaining the fixed points values
           end
       end
   end
   n=n+1;
   if ( n>2 && max(max(MaxMin-Pre))<1e-4) %Test for Convergence of images
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
    if ( n>2 && max(max(MaxMinWcWs-Pre))<1e-4)
        break
    end
    Pre=MaxMinWcWs;
end

subplot(323);
imshow(MaxMinWcWs,[]);
title('Using Maxima Minima Ws Wc');

%Fixing Max Min Saddles
[a,b]=size(img);
I=zeros(a,b);
sdd=zeros(a,b);
for i=1:a
    for j=1:b
        
        if ( ws(i,j)==0 && wc(i,j)==0 ) %Calculate Saddles in a at intersections of Watershed and Watercourse
            sdd(i,j)=1;
        end           
    end
end

ccmax=bwconncomp(sdd);
smax=regionprops(ccmax,'centroid');
centmax=cat(1,smax.Centroid);
centmax=round(centmax);
[a,~]=size(centmax);
sdd=zeros(size(img));
for i=1:a
    sdd(centmax(i,1),centmax(i,2))=1;
end
[a,b]=size(img);
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
    if( n>2 && max(max(MaxMinSaddle-Pre))<1e-4)
        break
    end
    Pre=MaxMinSaddle;
end

subplot(324);
imshow(MaxMinSaddle,[]);
title('Reconstructed using Max Min Saddle');

%Fixing only Edges
I=zeros(a,b);
ed=edge(img);  %Edge detection using default Sobel Edge Detector
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
    Ed=imfilter(Pre,f);
    for i=1:a
        for j=1:b
            if (I(i,j)~=0)
                Ed(i,j)=I(i,j);
            end
        end
    end
    n=n+1;
    if( n>2000)
        break
    end
    Pre=Ed;
end

subplot(325);
imshow(Ed,[]);
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
    MaxMinEd=imfilter(Pre,f);
    for i=1:a
        for j=1:b
            if (I(i,j)~=0)
                MaxMinEd(i,j)=I(i,j);
            end
        end
    end
    n=n+1;
    if( n>1000 )
        break
    end
    Pre=MaxMinEd;
end

subplot(326);
imshow(MaxMinEd,[]);
title('Reconstruction using Maxima Minima Edges');

%These terms will help calculate compression ratio for any method used
c1=a*b;
c2=length(find(maxima==1));%Points fixed by maxima
c3=length(find(minima==1));%Points fixed by minima
c4=length(find(ws==0));%Points fixed by watershed
c5=length(find(wc==0));%Points fixed by watercourse
c6=length(find(sdd==1));%Points fixed by saddles
c7=length(find(ed==1));%Points fixed by edges

            
