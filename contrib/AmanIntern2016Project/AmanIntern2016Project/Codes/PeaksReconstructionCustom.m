%{
This code aims to reconstruct the Peaks Funciton by retaining certain points and their values from
the original function.
We can vary which points we set, eg, every alternate, every fifth, every fiftieth etc.
The images are reconstructed by solving the Heat Equation.
The Convergence criteria is that the maximum difference between successive reconstructions be less
than 1e-4, though the user can change it as per their requirements. 
%}

I=peaks(50);

%f=fspecial('average',[3 3]);            %Averaging Filter
f=[0 1/4 0;1/4 0 1/4;0 1/4 0];          %Laplacian Filter 

subplot(2,2,1);
imagesc(I);
title('Original Image');

[a,b]=size(I);

%Setting every alternate Pixel
for i=1:a
    for j=1:b
        if ~(rem(j,2)==0 && rem(i,2)==0)
            I(i,j)=0;
        end
    end
end

n=1;
IR=I;
while 1
    IR=imfilter(IR,f);
    for i=1:a
        for j=1:b
            if (rem(j,2)==0 && rem(i,2)==0)
                IR(i,j)=I(i,j);
            end
            
        end
    end
    
    if (n>2 && max(max(IR-IO))<1e-4)
        break
    end
    n=n+1;  
    IO=IR;  
end

subplot(2,2,2);
imagesc(IR);
title('Reconstructed Image 2');

I=peaks(50);
%Setting every Fifth Pixel
for i=1:a
    for j=1:b
        if ~(rem(j,5)==0 && rem(i,5)==0)
            I(i,j)=0;
        end
    end
end

n=1;
IRR=I;
while 1
    IRR=imfilter(IRR,f);
    for i=1:a
        for j=1:b
            if (rem(j,5)==0 && rem(i,5)==0)
                IRR(i,j)=I(i,j);
            end
            
        end
    end
    n=n+1; 
    if (n>2 && max(max(IRR-IO))<1e-10)
        break
    end
     
    IO=IRR;  
end
subplot(2,2,3);
imagesc(IRR);
title('Reconstructed Image 5');

I=peaks(50);
%Setting every Tenth Pixel
for i=1:a
    for j=1:b
        if ~(rem(j,10)==0 && rem(i,10)==0)
            I(i,j)=0;
        end
    end
end

n=1;
IRRR=I;
while 1
    IRRR=imfilter(IRRR,f);
    for i=1:a
        for j=1:b
            if (rem(j,10)==0 && rem(i,10)==0)
                IRRR(i,j)=I(i,j);
            end
            
        end
    end
    
    if (n>2 && max(max(IRRR-IO))<1e-6)
        break
    end
    n=n+1;  
    IO=IRRR;  
end
      

subplot(2,2,4);
imagesc(IRRR);
title('Reconstructed Image from 10');




    
 
