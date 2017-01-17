%% ------------------------------------------------------------------------
function [xshock_det_list, image_list] = load_vox_ethz_detection(detection_result_folder, ...
  excluded_images)
% Load all object detection resuls from a detection result folder

if (nargin < 2)
  excluded_images = [];
end;



xshock_det_list = [];
image_list = [];

% collect names of all detection files
xml_det_files = dir(fullfile(detection_result_folder, 'xml_det_record+*.xml'));

%%%%%%%%%%%%%%%%%%%%%%%%%%
% temporary hack to get a pr graph for applelogos on applelogos
%%%xml_det_files = dir(fullfile(detection_result_folder, 'xml_det_record+applelogos_another.xgraph.0.prototype1+applelogos*.xml'));
%%%%%%%%%%%%%%%%%%%%%%%%%%

% parse the detection files one by one and concatenate the results
fprintf(1, '\n> Parsing xml_det_files[ %d ]: ', length(xml_det_files));


num_det_files = length(xml_det_files);
for i = 1 : num_det_files
  fprintf(1, ' %d', i);
  det_filename = xml_det_files(i).name;
  
  
  % determine name of the testing image
  last_plus = strfind(det_filename, '+');
  last_plus = last_plus(end);
  
  last_dot = strfind(det_filename, '.');
  last_dot = last_dot(end);
  
  image_name = det_filename(last_plus+1 : last_dot-1); 
  
  % check if this is one of the excluded_images
  iy = strmatch(image_name, excluded_images);
  if (~isempty(iy))
    continue;
  end;

  
  % save image name
  image_list{end+1} = image_name;
  
  
  det_file = fullfile(detection_result_folder, det_filename);
  [tree, treename] = xml_read(det_file);
  
  % concatenate the detections
  if (isfield(tree, 'xshock_det'))
    dets = tree.xshock_det;
  
%     % debugging - find the detection with the most confidence
%     num_dets = length(dets);
%     confidence = zeros(num_dets, 1);
%     for j = 1: num_dets
%       confidence(j) = dets(j).confidence;
%     end;
%     [Y, I] = max(confidence);
%     dets = dets(I);
  
    xshock_det_list = [xshock_det_list, dets];
  end;
  
  
  
end;

image_list = unique(image_list);

fprintf(1, '\n');



return;