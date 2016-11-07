clear all
close all

test_files=dir('test/*.png');


for t=1:length(test_files)
    I=imread(['test/' test_files(t).name]);
    
    seg=I>100;
    
    labeled_seg=bwlabel(seg);
    
    stats=regionprops(labeled_seg,'Area');
    
    
    [y,i]=max(struct2array(stats));
    
    region=labeled_seg==i;
    
    best_frag=bwfill(region,'holes');
    
    best_frag(1:2,:)=0;
    best_frag(end-1:end,:)=0;
    best_frag(:,end-1:end)=0;
    best_frag(:,1:2)=0;
    
    final_image(:,:,1)=best_frag*255;
    final_image(:,:,2)=best_frag*255;
    final_image(:,:,3)=best_frag*255;
    
    final_image=uint8(final_image);
    
    imwrite(final_image,test_files(t).name);
    
    clear final_image best_frag;
    
    
    
end
