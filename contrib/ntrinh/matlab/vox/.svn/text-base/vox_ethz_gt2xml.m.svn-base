function success = vox_ethz_gt2xml(groundtruth_file, category, xml_file)
% function status = vox_ethz_gt2xml(groundtruth_file, category, xml_file)
%
% This function loads an ETHZ groundtruth file and writes out an XML file
% compatible with VOX's groundtruth file format
% Example XML file
% <type name = "image">
% 	<description>
% 		<instance>
% 			<category>giraffes</category>
% 			<bndbox>
% 				<xmin>76</xmin>
% 				<ymin>21</ymin>
% 				<xmax>348</xmax>
% 				<ymax>355</ymax>
% 			</bndbox>
% 		</instance>
% 		<instance>
% 			<category>giraffes</category>
% 			<bndbox>
% 				<xmin>380</xmin>
% 				<ymin>189</ymin>
% 				<xmax>476</xmax>
% 				<ymax>309</ymax>
% 			</bndbox>
% 		</instance>
% 	</description>
% </type>
% 
% (c) Nhon Trinh
% Date: Jan 31, 2009
% 

%% checking arguments
if (nargin ~= 3)
  fprintf(2, 'ERROR: number of arguments is not 3. Use help for instruction.\n');
  success = 0;
  return;
end;


%% load the groudtruth file
gt_boxes = dlmread(groundtruth_file, ' ');

%% create the struct
gt = [];
gt.ATTRIBUTE.name = 'image';
for i = 1 : size(gt_boxes, 1)
  box = gt_boxes(i, :);
  
  % object instance
  instance = [];
  instance.category = category;
  instance.bndbox.xmin = box(1);
  instance.bndbox.ymin = box(2);
  instance.bndbox.xmax = box(3);
  instance.bndbox.ymax = box(4);
  
  % insert to the overall tree
  gt.description.instance(i) = instance;  
end;

%% Write out XML file using xml_io_tools library
xml_write(xml_file, gt, 'type');

success = 1;

return

