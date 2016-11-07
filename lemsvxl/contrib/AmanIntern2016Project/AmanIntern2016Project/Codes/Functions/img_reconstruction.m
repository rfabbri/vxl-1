function reconstructedimage = img_reconstruction(img,I,method)
%{
This function takes three inputs and returns the reconstruced image as output. Reconstruction is
based on the solution of the Heat Equation. 
The tolerance limit on the reconstructed image is the maximum difference between succesive
reconstructions must be less than 1e-4.

Inputs 
1)img = This is the input GRAYSCALE image which will specify the intensities at the points we want to fix.
2)I = This matrix is a logical matrix which has 1's at indices which we want to fix and 0's at
indices which we don't fix. the size of the matrix is the same as that of the image. Which points to
fix are determined prior to calling the function via another function.
3)method = This string is used to indicate how we want to reconstruct the image. It can either specify
'mean' or 'laplace' methods. The Mean method takes the average of a 8-connected neighbourhood as the
value of the center pixel. The Laplace method returns the central pixel value by taking a quarter of
the value of the pixels located directly above,below,left and right of the central pixel.

Output
The output will be the recontructed image. 
%}

%This is used to specify which method of reconstruction the user wants to use, ie, mean or laplace. 
switch method
    case 'mean'
        f=fspecial('average',[3 3]); 
    case 'laplace'
        f=[0 1/40;1/4 0 1/4;0 1/4 0];
end

n=1;
I=uint8(I);
pre=I; 
[a,b]=size(img);

while 1
   recimg=imfilter(pre,f);%Applying Filter to image
   for i=1:a
       for j=1:b
           if ( I(i,j)~=0 )
               recimg(i,j)=img(i,j); %Retaining the fixed points intensities
           end
       end
   end
   n=n+1;
   if ( n>2 && max(max(recimg-pre))<1e-4) %Test for Convergence (Tolerance)
      break
   end
   pre=recimg;
end 

reconstructedimage=recimg;
