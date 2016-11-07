plotfig3d = 98;
figure(plotfig3d);
clf;
axis equal;
hold on;

n_of_blocks = 8;
n_of_views = 4; % views per block
display_images = false;

vpts2d = cell(n_of_blocks,n_of_views);
vpts3d = cell(n_of_blocks,n_of_views);
vcameras= cell(n_of_blocks,n_of_views);
vcenters= cell(n_of_blocks,n_of_views);
vdirections= cell(n_of_blocks,n_of_views);
vresiduals = cell(n_of_blocks,n_of_views);
vimgnames = cell(n_of_blocks,n_of_views);


for nb=1:n_of_blocks
  for nv=1:n_of_views
    nblock = num2str(nb);
    nview = num2str(nv);
    showpts;

    vpts2d{nb,nv} = pts2d;
    vpts3d{nb,nv} = pts3d;
    vcameras{nb,nv} = cam;
    vdirections{nb,nv} = fvec;
    vcenters{nb,nv} = fvec;
    vresiduals{nb,nv} = derr;
    vimgnames{nb,nv} = [mypath img_f];
  end
end
