%{
Code to reconstruct Cameraman Image by fixing points using the following methods
1)Maxima and Minima
2)Max, Min, Watershed, Watercourse
3)Max, Min, Saddles
The Indices of respective points are retained and their intensities are fixed as that in the
original image or the blurred image.
The images are reconstructed by solving the Heat Equation.
The Convergence criteria is that the maximum difference between successive reconstructions be less
than 1e-4, though the user can change it as per their requirements. 
Variable to calculate compresssion ratios are also calculated.
%}

%Read the Image
oimg=imread('E:\USA Trip\Work\Original Images\Cameraman.tif');
img=oimg;

% f=fspecial('average',[3 3]);     %Averaging Filter
f=[0 1/4 0;1/4 0 1/4;0 1/4 0];    %Laplacian Filter

subplot(221);
imshow(img);
title('Original Image');
% fil=fspecial('gaussian',[7 7],1);  %Filter Image to reduce number of critical points
% img=imfilter(img,fil);

%%Reducing number of critical points using connected components 
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
ws=watershed(imgd);                         %Computer Watershed Matrix
wc=watershed(-imgd);                        %Computer Watercourse Matrix

nbh=[];
[a,b]=size(img);
%Fixing Maxima and Minima
I=zeros(a,b);
for i=1:a
    for j=1:b
        if ( maxima(i,j)==1 || minima(i,j)==1 )
            I(i,j)=oimg(i,j);                  %Fixing Points of either maxima or minima
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
   if ( n>2 && max(max(MaxMin-Pre))<1e-10)
      break
   end
   Pre=MaxMin;
end 

subplot(222);
imshow(MaxMin,[]);
title('Reconstructed Image Maxima and Minima');

%Fixing Maxima Minima Watershed and Waterscourse
I=zeros(a,b);
for i=1:a
    for j=1:b
        if (maxima(i,j)==1 || minima(i,j)==1 || ws(i,j)==0 || wc(i,j)==0 )
            I(i,j)=oimg(i,j);
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
                MaxMinWcWs(i,j)=oimg(i,j);
            
            end
        end
    end
    n=n+1;
    if ( n>2 && max(max(MaxMinWcWs-Pre))<1e-4)
        break
    end
    Pre=MaxMinWcWs;
end

subplot(223);
imshow(MaxMinWcWs,[]);
title('Using Maxima Minima Ws Wc');
[a,b]=size(img);

%Fixing Max Min Saddles
I=zeros(a,b);
sdd=zeros(a,b);              %Saddle Matrix 
for i=1:a
    for j=1:b
        
        if ( ws(i,j)==0 && wc(i,j)==0 )
            sdd(i,j)=1;                    %Saddle Points are taken at intersections of watershed and watercourse lines
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
            I(i,j)=oimg(i,j);
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
                MaxMinSaddle(i,j)=oimg(i,j);
            end
        end
    end
    n=n+1;
    if( n>2 && max(max(MaxMinSaddle-Pre))<1e-4)
        break
    end
    Pre=MaxMinSaddle;
end

subplot(224);
imshow(MaxMinSaddle,[]);
title('Reconstructed using Max Min Saddle');


%These variable can be used to calculate the compression ratios

c1=length(find(maxima==1));     %Number of points fixed as Maxima
c2=length(find(minima==1));     %Number of points fixed as Minima       
c3=length(find(ws==0));         %Number of points fixed as Watershed
c4=length(find(wc==0));         %Number of points fixed as Watercourse
c5=length(find(sdd==1));        %Number of points fixed as Saddles
c6=a*b;                         %Number of Points in Image


            
