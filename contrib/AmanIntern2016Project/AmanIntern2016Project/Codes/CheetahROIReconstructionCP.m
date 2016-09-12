%{
Code to reconstruct a particular custom Region of Interest of Cheetah Image by fixing points using 
the following methods
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


img=imread('cheetah.jpg');
img=rgb2gray(img);
%Define the custom Region of Interest
img=imcrop(img,[87 6 150 115]);

% f=fspecial('average',[3 3]);          %Averaging Filter
f=[0 1/4 0;1/4 0 1/4;0 1/4 0];          %Laplacian Filter

subplot(221);
imshow(img);
title('Original Image');

% fil=fspecial('gaussian',[7 7],1);       %Filter Image to reduce number of critical points
% img=imfilter(img,fil);

[a, b]=size(img);

maxima=imregionalmax(img);
minima=imregionalmin(img);
imgd=double(img);
ws=watershed(imgd);
wc=watershed(-imgd);
nbh=[];

%Fixing Maxima and Minima
I=zeros(a,b);
for i=1:a
    for j=1:b
        if ( maxima(i,j)==1 || minima(i,j)==1 )
            I(i,j)=img(i,j);
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
   if ( n>2 && max(max(MaxMin-Pre))<1e-4)
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

subplot(223);
imshow(MaxMinWcWs,[]);
title('Using Maxima Minima Ws Wc');

%Fixing Max Min Saddles
I=zeros(a,b);
sdd=zeros(a,b);        %Saddle Matrix
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