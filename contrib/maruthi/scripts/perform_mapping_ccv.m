function [overall_stats]=perform_mapping_ccv(dc_file,model_file,query_esf_file)


cub_prefix='/users/mnarayan/scratch/CUB_200_2011/images';


model_images=textread(model_file,'%s\n');
query_images{1}=query_esf_file;

[mapping,query_points]=read_dc_file(dc_file);

overall_stats=struct;

for m=1:length(model_images)
    
    [path,name,ext]=fileparts(model_images{m});
    
    mimg=imread([cub_prefix '/' name '.jpg']);
    bw_image=imread([path '/' name '.png']);
    bw_image=bw_image(:,:,1)>0;
    
    mask=uint8(bw_image);
    mask_complement=uint8(~bw_image*255);
    
    mimg(:,:,1)=mimg(:,:,1).*mask+mask_complement;
    mimg(:,:,2)=mimg(:,:,2).*mask+mask_complement;
    mimg(:,:,3)=mimg(:,:,3).*mask+mask_complement;
    
    
    
    underscores=strfind(name,'_');
    model_category=name(1:underscores(end-1)-1);
    
    
    if ( ~isfield(overall_stats,model_category))
        eval(['overall_stats.' model_category '={};']);
    end
    
    for q=1:length(query_images)
        
        [path,name,ext]=fileparts(query_images{q});
        qimg=imread([cub_prefix '/' name '.jpg']);
        
        mp=mapping{m,q};
        op=query_points{q};
        
        red_values=interp2(double(mimg(:,:,1)),mp(:,1),mp(:,2),'cubic',255);
        green_values=interp2(double(mimg(:,:,2)),mp(:,1),mp(:,2),'cubic',255);
        blue_values=interp2(double(mimg(:,:,3)),mp(:,1),mp(:,2),'cubic',255);
        
        plane1=ones(size(qimg(:,:,1)))*255;
        plane2=plane1;
        plane3=plane1;
        
        indices=sub2ind(size(plane1),op(:,2),op(:,1));
        
        plane1(indices)=red_values;
        plane2(indices)=green_values;
        plane3(indices)=blue_values;
        
        
        new_image(:,:,1)=plane1;
        new_image(:,:,2)=plane2;
        new_image(:,:,3)=plane3;
        
        new_image=uint8(new_image);
        
        value=getfield(overall_stats,model_category);
        index=length(value)+1;
        eval(['overall_stats.' model_category '{' num2str(index) '}=new_image;']);

        
        
        
    end
end
