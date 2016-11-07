%Add path of function which we use to extract points
addpath('E:\USA Trip\Work\Functions');
%Data read base path
base_dir = 'E:\USA Trip\Work\Dataset 1';
%Get the names of all the folders in this path 
fold = dir(base_dir);
%Get rid of the folder itself and any files 
fol = {};
%Create Smoothing Gaussian Filter
filter=fspecial('gaussian',[15 15],2);

for k=1:length(fold)
    if ((fold(k).isdir) && fold(k).name(1)~='.')
        fol{end+1} = fold(k).name;
    end
end

origimg=cell(1,18);
f=cell(1,18);
d=cell(1,18);

% for i=3:length(fol)
%     acquire_dir=fullfile(base_dir,fol{i});
%     mvimg=dir(acquire_dir);
%     acimg={};
%     for k=1:length(mvimg)
%         if (~(mvimg(k).isdir) && mvimg(k).name(1)~='.')
%            acimg{end+1} = mvimg(k).name;
%         end
%     end
%     
%     for l=1:53
%         mvpath=fullfile(acquire_dir,acimg{l});
%         finalpath=fullfile(base_dir,fol{1});
%         copyfile(mvpath,finalpath);
%     end
% end

for i=2
    query_dir=fullfile(base_dir, fol{i});
    imgd=dir(query_dir);
    imgsel={};
    for k=1:length(imgd)
        if (~(imgd(k).isdir) && imgd(k).name(1)~='.')
           imgsel{end+1} = imgd(k).name;
        end
    end
end
    
for k=1:length(imgsel)
    impath=fullfile(query_dir,imgsel{k});
    img=vl_imreadgray(impath);
    origimg{k}=imgsel{k};
    img=imfilter(img,filter);
    img=single(img);
    ws=extract_points(img,'watershed');
    wc=extract_points(img,'watercourse');
    [a,b]=size(img);
    sdd=zeros(a,b);
    for l=1:a
        for j=1:b
            if (ws(l,j)==0 && wc(l,j)==0)
                sdd(l,j)=1;
            end
        end
    end
    ccmax=bwconncomp(sdd);
    smax=regionprops(ccmax,'centroid');
    centmax=cat(1,smax.Centroid);
    centmax=round(centmax);
    [a,~]=size(centmax);
    sdd=zeros(size(img));
    for l=1:a
        sdd(centmax(l,1),centmax(l,2))=1;
    end
    [a,b]=size(img);
    l=0;
    for i=1:a
        for j=1:b
            if (sdd(i,j)==1)
                l=l+1;
                f{k}(1,l)=i;
                f{k}(2,l)=j;
            end
        end
    end
    f{k}(3,:)=1;
    f{k}(4,:)=0;
    [f{k},d{k}]=vl_sift(img,'frames',f{k},'orientations');  
end

matimg=cell(1,18);
name=cell(1,18);

matched=0;

query_dir=fullfile(base_dir, fol{1});
imgd=dir(query_dir);
imgsel={};
for k=1:length(imgd)
    if (~(imgd(k).isdir) && imgd(k).name(1)~='.')
        imgsel{end+1} = imgd(k).name;
    end
end
    
for i=1:18
    matched=0;
    for k=1:length(imgsel)
        mf=[];
        impath=fullfile(query_dir,imgsel{k});
        img=vl_imreadgray(impath);
        img=imfilter(img,filter);
        img=single(img);
        ws=extract_points(img,'watershed');
        wc=extract_points(img,'watercourse');
        [a,b]=size(img);
        sddf=zeros(a,b);
        for l=1:a
            for j=1:b
                if (ws(l,j)==0 && wc(l,j)==0)
                    sddf(l,j)=1;
                end
            end
        end
        ccmax=bwconncomp(sddf);
        smax=regionprops(ccmax,'centroid');
        centmax=cat(1,smax.Centroid);
        centmax=round(centmax);
        [a,~]=size(centmax);
        sddf=zeros(size(img));
        for l=1:a
            sddf(centmax(l,1),centmax(l,2))=1;
        end
        l=0;
        [a,b]=size(img);
        for m=1:a
            for j=1:b
                if (sddf(m,j)==1)
                    l=l+1;
                    mf(1,l)=m;
                    mf(2,l)=j;
                end
            end
        end
        mf(3,:)=1;
        mf(4,:)=0;
        [mf,md]=vl_sift(img,'frames',mf,'orientations');
    
        [matches,scores]=vl_ubcmatch(d{i},md);
        if (length(matches) > matched)
           matched=length(matches); 
           matimg{i}=img;
           name{i}=imgsel{k};
        end
    end  
end

suc1=0;
suc2=0;
unsuc=0;

for i=1:18
    if (origimg{i}(4:7)==name{i}(4:7))
        suc1=suc1+1;
        if(abs(str2double(origimg{i}(9:10))-str2double(name{i}(9:10)))<3)
            suc2=suc2+1;
        end
    else
        unsuc=unsuc+1;
    end
end


