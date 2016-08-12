%Data read base path
base_dir = 'E:\USA Trip\Work\Dataset 1';
%Get the names of all the folders in this path 
fold = dir(base_dir);
%Get rid of the folder itself and any files 
fol = {};
%Create Gaussian smoothing filter 
fil=fspecial('gaussian',[15 15],2);

for k=1:length(fold)
    if ((fold(k).isdir) && fold(k).name(1)~='.')
        fol{end+1} = fold(k).name;
    end
end

origimg=cell(1,18);
f=cell(1,18);
d=cell(1,18);

%%This script is to move images from database to query folder
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
%     for l=1:50
%         mvpath=fullfile(acquire_dir,acimg{l});
%         finalpath=fullfile(base_dir,fol{1});
%         copyfile(mvpath,finalpath);
%     end
% end

%Read images name in query folder
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

%%Measure image properties
for k=1:length(imgsel)
    impath=fullfile(query_dir,imgsel{k});
    img=vl_imreadgray(impath);
    %img=imfilter(img,fil);
    img=single(img);
    origimg{k}=img;
    [f{k},d{k}]=vl_sift(img);    
end

matimg=cell(1,6);
name=cell(1,6);

query_dir=fullfile(base_dir, fol{1});
imgd=dir(query_dir);
imgsel={};
for k=1:length(imgd)
    if (~(imgd(k).isdir) && imgd(k).name(1)~='.')
       imgsel{end+1} = imgd(k).name;
    end
end

%%This is the matching on database folder 
for i=1:18
    matched=0;
    for k=1:length(imgsel)
        impath=fullfile(query_dir,imgsel{k});
        img=vl_imreadgray(impath);
        img=imfilter(img,fil);
        img=single(img);
        [mf,md]=vl_sift(img);
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



