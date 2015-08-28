function []=d2c_classify_ccv(test_file)


run /users/mnarayan/vlfeat-0.9.20/toolbox/vl_setup


colorspace=1;
scales=[4 8 12 16];
alpha=0.5;

load /users/mnarayan/scratch/fgc_shape_align/train_data/training_pca_128_gmm_512.mat

[path,name,ext]=fileparts(test_file);
data=load([name '.mat']);

orig_image=imread(data.qimg_path);
bw_image_path=strrep(test_file,'.esf','.png');

bw_image=imread(bw_image_path);
bw_image=bw_image(:,:,1)>0;

mask=uint8(bw_image);
mask_complement=uint8(~bw_image*255);

orig_image(:,:,1)=orig_image(:,:,1).*mask+mask_complement;
orig_image(:,:,2)=orig_image(:,:,2).*mask+mask_complement;
orig_image(:,:,3)=orig_image(:,:,3).*mask+mask_complement;
   
query_fvs=compute_color_sift_pca_fv(orig_image,data.sampled_points,colorspace,scales,means,covariances,priors,mapping,alpha);

fields=fieldnames(data.output);
d2c=zeros(length(fields),1);

for f=1:length(fields)
    values=getfield(data.output,fields{f});
    
    temp_data=zeros(size(values,2),size(data.sampled_points,1));
    
    for s=1:size(values,2)
        orig_image=uint8(values{s});
        test_fvs=compute_color_sift_pca_fv(orig_image,data.sampled_points,colorspace,scales,means,covariances,priors,mapping,alpha);
        l2dist=sum((query_fvs-test_fvs).^2,1);
        temp_data(s,:)=l2dist;
        
    end
    
    
    d2c(f,1)=sum(min(temp_data,[],1));
    
end

save([name '_d2c.mat'],'d2c');



function fvs=compute_color_sift_pca_fv(orig_image,sampled_points,colorspace,scales,means,covariances,priors,mapping,alpha)


[L,a,b] = RGB2Lab(orig_image(:,:,1),orig_image(:,:,2),orig_image(:,:,3));

L=vl_imsmooth(L,1.0);
a=vl_imsmooth(a,1.0);
b=vl_imsmooth(b,1.0);

[Ix, Iy] = vl_grad(L) ;
mod      = sqrt(Ix.^2 + Iy.^2) ;
ang      = atan2(Iy,Ix) ;
grd      = shiftdim(cat(3,mod,ang),2) ;
grdL      = single(grd) ;


[Ix, Iy] = vl_grad(a) ;
mod      = sqrt(Ix.^2 + Iy.^2) ;
ang      = atan2(Iy,Ix) ;
grd      = shiftdim(cat(3,mod,ang),2) ;
grda      = single(grd) ;


[Ix, Iy] = vl_grad(b) ;
mod      = sqrt(Ix.^2 + Iy.^2) ;
ang      = atan2(Iy,Ix) ;
grd      = shiftdim(cat(3,mod,ang),2) ;
grdb     = single(grd) ;

% [L,a,b] = RGB2Lab(orig_image(:,:,1),orig_image(:,:,2),orig_image(:,:,3));
% 
% %Compute L gradient
% [LMag,LDir]=imgradient(L);
% grdL      = shiftdim(cat(3,LMag,deg2rad(LDir)),2) ;
% grdL      = single(grdL) ;
% 
% %Compute a gradient
% [aMag,aDir]=imgradient(a);
% grda      = shiftdim(cat(3,aMag,deg2rad(aDir)),2) ;
% grda      = single(grda) ;
% 
% %Compute b gradient
% [bMag,bDir]=imgradient(b);
% grdb      = shiftdim(cat(3,bMag,deg2rad(bDir)),2) ;
% grdb      = single(grdb) ;

color_sifts=zeros(384,size(sampled_points,1)*length(scales));
for s=1:length(scales)
    radius=ones(1,size(sampled_points,1))*scales(s);
    orient=zeros(1,size(sampled_points,1));
    keypoints=[sampled_points(:,2).'; sampled_points(:,1).' ; radius ; orient;];
    
    sift_L=vl_siftdescriptor(grdL,keypoints,'Magnif',1.0,'FloatDescriptors');
    sift_a=vl_siftdescriptor(grda,keypoints,'Magnif',1.0,'FloatDescriptors');
    sift_b=vl_siftdescriptor(grdb,keypoints,'Magnif',1.0,'FloatDescriptors');
    
    temp=[sift_L ; sift_a ; sift_b];
    
    start=1+size(sampled_points,1)*(s-1);
    stop=size(sampled_points,1)*s;
    
    color_sifts(:,start:stop)=temp;
    
   
end



color_sifts=color_sifts-repmat(mapping.mean',[1 size(color_sifts,2)]);
color_sifts=mapping.M.'*color_sifts;



fvs=zeros(2*numel(covariances),size(sampled_points,1));

for b=1:size(sampled_points,1)
    
    set=color_sifts(:,b:size(sampled_points,1):end); 
    raw=vl_fisher(set,means,covariances,priors);
    pln=sign(raw).*(abs(raw).^alpha);
    fvs(:,b)=pln./norm(pln);
end
