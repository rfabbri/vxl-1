% This script analyzes the distribution of root node's locations for the
% xshock detection results
% (c) Nhon Trinh
% Date: July 6, 2009

%% Input 

det_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-experiments\exp_71-applelogos_prototype1-ethz-v0_86-fixed_bug';
det_record_fname = 'xml_det_record+applelogos_another.xgraph.0.prototype1+applelogos_bigwindow.xml';

root_node_vid = 3;
image_width = 1036;
image_height = 777;

%% Process

[det_tree, det_treename] = xml_read(fullfile(det_folder, det_record_fname));

% concatenate the detections
if (~isfield(det_tree, 'xshock_det'))
  fprintf(1, '\nERROR: Wrong XML file type.\n');
  return;
end;

% iterate thru the list of detections and collect the (x,y) coordinate of
% the root node
num_dets = length(det_tree.xshock_det);

fprintf(1, 'Number of detections = %d\n', num_dets);

root_xy = zeros(num_dets, 2);
confidence = zeros(num_dets, 1);
for k = 1 : num_dets
  fprintf(1, 'Processing detection %d...', k);
  confidence(k) = det_tree.xshock_det(k).confidence;
  
  % full path to the xgraph xml file
  xgraph_fname = det_tree.xshock_det(k).xgraph_xml(2:end-1);
  xgraph_file = fullfile(det_folder, xgraph_fname);

  % load the xgraph xml
  [xgraph_tree, xgraph_treename] = xml_read(xgraph_file);

  % nail down the root node
  num_nodes = length(xgraph_tree.shock_node_list.shock_node);
  root_node = [];
  for i = 1 : num_nodes
    xnode = xgraph_tree.shock_node_list.shock_node(i);
    if (xnode.ATTRIBUTE.id == root_node_vid)
      root_node = xnode;
      break;
    end;
  end;
  if (isempty(root_node))
    fprintf(1, 'ERROR: could not locate root node.\n');
    continue;
  end;
  
  root_xy(k, 1) = root_node.ATTRIBUTE.x;
  root_xy(k, 2) = root_node.ATTRIBUTE.y;
  fprintf(1, '[ OK ]\n');
end;

im = zeros(image_height, image_width);
root_pos = round(root_xy)+1;
im(sub2ind(size(im), root_pos(:, 2), root_pos(:, 1))) = confidence;
mesh(im);
colormap(hsv);
colorbar;

% root_pos = round(root_xy)+1;
% figure;
% %axis image;
% hold on;
% cc = hsv(num_dets);
% for kk = 1 : num_dets
%   plot(root_pos(kk, 1), root_pos(kk, 2), 'o');
% end;
% %hold off;



