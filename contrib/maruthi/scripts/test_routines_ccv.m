function []=test_routines_ccv(test_file,train_file)


%Path to CUB 200 2011 folder image folder
cub_prefix='/users/mnarayan/scratch/CUB_200_2011/images';

dc_bin_path='/users/mnarayan/scratch/fgc_shape_align/test_dc';

stride=8;

disp(['Working on ' test_file]);

[path,name,ext]=fileparts(test_file);
bin_file=dir([dc_bin_path '/*' name '*.bin']);

if ( isempty(bin_file))
    return;
end

bin_name=bin_file(1).name;
index=strfind(bin_name,'_vs_');
test_name=bin_name((index+4):end);
underscores=strfind(test_name,'_');

test_category=test_name(1:underscores(end-3)-1);
qimg_bw=imread([path '/' name '.png']);
qimg_path=[cub_prefix '/' name '.jpg'];

[sampled_points]=sample_image(qimg_bw(:,:,1)>0,stride);

[output]=perform_mapping_ccv([dc_bin_path '/' bin_file(1).name],train_file,test_file);


save([name '.mat'],'sampled_points','output','qimg_path','test_category');


function [sampled_points]=sample_image(bw_image,stride)

borders=bwboundaries(bw_image);
xcoord=borders{1}(:,1);
ycoord=borders{1}(:,2);

[X,Y]=meshgrid(1:stride:size(bw_image,1),1:stride:size(bw_image,2));
sample_points=[X(:) Y(:)];
points_in_poly=inpolygon(sample_points(:,1),sample_points(:,2),xcoord,ycoord);
sampled_points=sample_points(points_in_poly,:);




