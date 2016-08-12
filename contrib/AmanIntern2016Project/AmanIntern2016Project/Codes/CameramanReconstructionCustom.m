%{
This code aims to reconstruct the Cameraman image by retaining certain points and their intensities
from the original image.
We can vary which points we set, eg, every alternate, every fifth, every fiftieth etc.
The images are reconstructed by solving the Heat Equation.
The Convergence criteria is that the maximum difference between successive reconstructions be less
than 1e-4, though the user can change it as per their requirements. 
%}

img=imread('cameraman.tif');

f=fspecial('average',[3 3]);            %Averaging Filter
% f=[0 1/4 0;1/4 0 1/4;0 1/4 0];          %Laplacian Filter 

subplot(3,2,1);
imagesc(img);
title('Original Image');

[a,b]=size(img);

I=imread('cameraman.tif');
%Setting every alternate Pixel
for i=1:a
    for j=1:b
        if ~(rem(j,2)==0 && rem(i,2)==0)
            I(i,j)=0;
        end
    end
end

n=1;
IO=I;
while 1
    IR=imfilter(IO,f);
    for i=1:a
        for j=1:b
            if (rem(j,2)==0 && rem(i,2)==0)
                IR(i,j)=img(i,j);
            end
            
        end
    end
    
    n=n+1;  
     
    if (n>2 && max(max(IR-IO))<0.00001)
        break
    end
    IO=IR;
end

subplot(3,2,2);
imagesc(IR);
title('Reconstructed Image 2');


I=imread('cameraman.tif');
%Setting every Tenth Pixel
for i=1:a
    for j=1:b
        if ~(rem(j,10)==0 && rem(i,10)==0)
            I(i,j)=0;
        end
    end
end

n=1;
IO=I;
while 1
    IRR=imfilter(IO,f);
    for i=1:a
        for j=1:b
            if (rem(j,10)==0 && rem(i,10)==0)
                IRR(i,j)=img(i,j);
            end
            
        end
    end
    n=n+1;    
    if (n>2 && max(max(IRR-IO))<0.00001)
        break
    end
     
    IO=IRR;  
end
      
subplot(3,2,3);
imagesc(IRR);
title('Reconstructed Image from 10');


I=imread('cameraman.tif');

%Setting every Twentieth Pixel
for i=1:a
    for j=1:b
        if ~(rem(j,20)==0 && rem(i,20)==0)
            I(i,j)=0;
        end
    end
end

n=1;
IO=I;
while 1
    IRRR=imfilter(IO,f);
    for i=1:a
        for j=1:b
            if (rem(j,20)==0 && rem(i,20)==0)
                IRRR(i,j)=I(i,j);
            end
            
        end
    end
    
    
    n=n+1;  
    if (n>2 && max(max(IRRR-IO))<1e-4)
        break
    end
    IO=IRRR;  
end
      
subplot(3,2,4);
imagesc(IRRR);
title('Reconstructed Image from 20');

I=imread('cameraman.tif');

%Setting every Fiftieth Pixel
for i=1:a
    for j=1:b
        if ~(rem(j,50)==0 && rem(i,50)==0)
            I(i,j)=0;
        end
    end
end

n=1;
IO=I;
while 1
    IRRRR=imfilter(IO,f);
    for i=1:a
        for j=1:b
            if (rem(j,50)==0 && rem(i,50)==0)
                IRRRR(i,j)=I(i,j);
            end
            
        end
    end
    n=n+1; 
    if (n>2 && max(max(IRRRR-IO))<1e-4)
        break
    end
     
    IO=IRRRR;  
end
      
subplot(3,2,5);
imagesc(IRRRR);
title('Reconstructed Image from 50');

    
 
