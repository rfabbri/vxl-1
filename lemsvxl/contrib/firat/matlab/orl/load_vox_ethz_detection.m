%% ------------------------------------------------------------------------
function [xshock_det_list, image_list] = load_vox_ethz_detection(detection_result_folder, ...
  excluded_images)
% Load all object detection resuls from a detection result folder

firat_bool = 0;

if(firat_bool)
    %load('~/Desktop/mugs_det.mat')
    %load('~/Desktop/mugs-bb_det.mat')
    %load('~/Desktop/bottles_det.mat')
    %load('~/Desktop/bottles_be_det.mat')
    %load('~/Desktop/cars_det.mat')
    %load('~/Desktop/swans_det.mat')
    %load('~/Desktop/giraffes_det.mat')
    %load('~/Desktop/applelogos_det.mat')
    %load('~/Desktop/applelogos_be_det.mat')
    %load('~/Desktop/applelogos_ee_det.mat')
    %load('~/Desktop/mugs_be_det.mat')
    %load('~/Desktop/swans_be_det.mat')
    %load('~/Desktop/swans_ee_det.mat')
    %load('~/Desktop/giraffes_be_det.mat')
    return
end


if (nargin < 2)
  excluded_images = [];
end;



%xshock_det_list = [];
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
xshock_det_list = cell(1, num_det_files);
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
  
  confidence_threshold = 0;
  
  % concatenate the detections
  if (isfield(tree, 'xshock_det'))
    dets = tree.xshock_det; 
    num_dets = length(dets);
    confidence = zeros(num_dets, 1);
    xscale = zeros(num_dets, 1);
    for j = 1: num_dets
      confidence(j) = dets(j).confidence;
    end;    
    xshock_det_list{i} = dets(confidence > confidence_threshold);
  end;
  
  
  
end
xshock_det_list(cellfun(@(xshock_det_list) isempty(xshock_det_list),xshock_det_list))=[];
xshock_det_list = cell2mat(xshock_det_list);
image_list = unique(image_list);

fprintf(1, '\n');

if(~firat_bool)
    %save('~/Desktop/mugs_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/mugs-bb_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/bottles_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/bottles_be_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/cars_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/swans_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/giraffes_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/applelogos_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/applelogos_be_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/applelogos_ee_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/mugs_be_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/swans_be_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/swans_ee_det.mat', 'xshock_det_list', 'image_list')
    %save('~/Desktop/giraffes_be_det.mat', 'xshock_det_list', 'image_list')
end
return;
