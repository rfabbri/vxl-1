function [thresholded_sift_descriptors]=compute_dense_sift_wrapper(imageFile,grad_thresh,spatial_scale,grid_spacing)

disp(['Computing Dense sifts at ' num2str(spatial_scale) ' scale with gs of ' num2str(grid_spacing) ' with grad thresh of ' num2str(grad_thresh) ])
disp(' ')
 
%Define params
patch_size=16;

%scale pictures
base_size=300;

%Read in image
I1 = imread(imageFile); 

%Resize image to 300 in longest dimension
I1=imresize(I1,base_size/length(I1));

%Apply scaling
if ( spatial_scale ~= 1.0 )
   disp('Resizing to spatial scale');
   I1=imresize(I1,spatial_scale); 
end
            
[sift_arr, grid_x, grid_y,grad_mag] = dense_sift(I1, ...
    patch_size, ...
    grid_spacing);

sifts=double(cat(3,sift_arr,grid_y,grid_x));
sifts(:,:,(end-1))= sifts(:,:,(end-1))/(size(I1,1));
sifts(:,:,(end))  = sifts(:,:,(end))/(size(I1,2));

%normalize grad_mag
norm_grad_mag=grad_mag./(max(max(grad_mag)));

% Create 2d matrix

numb_dims   = size(sifts,3);

sift_descriptors=[];

for planes=1:numb_dims
    column_dim=sifts(:,:,planes);
    sift_descriptors=[ sift_descriptors column_dim(:) ];
end

coords=[grid_y(:) grid_x(:)];

thresholded_sift_descriptors=[];

for k=1:length(sift_descriptors)
   
    grad_value=norm_grad_mag(coords(k,1),coords(k,2));
    
    if ( grad_value > grad_thresh )
        
       thresholded_sift_descriptors=[thresholded_sift_descriptors ; sift_descriptors(k,:)]; 
    end
    
    
    
end

disp(['Total number of sifts is ' num2str(length(thresholded_sift_descriptors)) ]);

