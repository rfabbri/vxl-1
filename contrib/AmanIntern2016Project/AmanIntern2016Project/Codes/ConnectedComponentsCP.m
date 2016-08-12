%{
This code aims to compute the reduction in maxima and minima points of an Image by finding connected
components and only fixing their centroids.
%}

img=imread('E:\USA Trip\Work\Original Images\Lena.tiff');
img=rgb2gray(img);

maxima=imregionalmax(img);
minima=imregionalmin(img);

ccmax=bwconncomp(maxima);           %Struct of maxima
ccmin=bwconncomp(minima);           %Struct of minima

smax=regionprops(ccmax,'centroid');     %Contains Centroid indices
smin=regionprops(ccmin,'centroid');

centmax=cat(1,smax.Centroid);
centmin=cat(1,smin.Centroid);

centmax=round(centmax);                 %Rounding off values to get integer indices
centmin=round(centmin);

[a,~]=size(centmax);
maximacc=zeros(size(img));

for i=1:a
    maximacc(centmax(i,1),centmax(i,2))=1;
end

[a,~]=size(centmin);
minimacc=zeros(size(img));

for i=1:a
    minimacc(centmin(i,1),centmin(i,2))=1;
end


length(find(maxima==1))               %Number of maxima 
length(find(maximacc==1))             %Number of maxima after removing connceted components
length(find(minima==1))               %Number of minima   
length(find(minimacc==1))             %Number of minima after removing connected components


