%Script to find the variation in number of maxima and minima with succesive gaussian filters of
%varying sigma values
 
%Read the Image
I=imread('E:\USA Trip\Work\Original Images\Cameraman.tif');

if ndims(I)==3
    % matrix is three dimensional, convert to grayscale
    I=rgb2gray(I);
end


% maxf=imregionalmax(I);
% minf=imregionalmin(I);
% [rowmax,colmax]=find(maxf==1);
% [rowmin,colmin]=find(minf==1);

%Set range of i such that 6*(maximum value) equals the size of the image
i=0.5:0.5:45;

%Creating cells for storing the maxima,minima points etc for every iteration 
max=cell(length(i),1);                  
min=cell(length(i),1);                  
nmax=zeros(1,length(i));                    
nmin=zeros(1,length(i));
img=cell(1,length(i));
rows1=cell(length(i),1);
rows2=cell(length(i),1);
cols1=cell(length(i),1);
cols2=cell(length(i),1);
k=1;

for i=0.5:0.5:45
    %Create Gaussian Filter
    f=fspecial('gaussian',[ceil(6*i) ceil(6*i)],i);
    img{k}=imfilter(I,f);
    max{k,1}=imregionalmax(img{k});
    min{k,1}=imregionalmin(img{k});
    %Reducing maxima and minma by connected component analysis
    ccmax=bwconncomp(max{k,1});
    ccmin=bwconncomp(min{k,1});
    smax=regionprops(ccmax,'centroid');
    smin=regionprops(ccmin,'centroid');
    centmax=cat(1,smax.Centroid);
    centmin=cat(1,smin.Centroid);
    %Convert centmax/min using uint8 cuts of pixel values for larger images, hence use round
    centmax=round(centmax);
    centmin=round(centmin);
    [a,~]=size(centmax);
    maximacc=zeros(size(I));
    for j=1:a
        maximacc(centmax(j,1),centmax(j,2))=1;
    end
    [a,b]=size(centmin);
    minimacc=zeros(size(I));
    for j=1:a
        minimacc(centmin(j,1),centmin(j,2))=1;
    end
    
    nmax(k)=length(find(maximacc==1));
    nmin(k)=length(find(minimacc==1));
    %Store the indices of maxima and minma points
    [rows1{k}, cols1{k}]=find(maximacc==1);
    [rows2{k}, cols2{k}]=find(minimacc==1);
    k=k+1;
end

i=0.5:0.5:45;
plot(i,nmax);
hold on
plot(i,nmin);
legend('nmax','nmin');



