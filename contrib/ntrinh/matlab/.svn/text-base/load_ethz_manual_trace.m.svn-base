function gt_ext = load_ethz_manual_trace(gt, manual_trace_folder)
% gt_ext = load_ethz_manual_trace(gt, manual_trace_folder)
% (c) Nhon Trinh
% Date: April 30, 2009
  
gt_ext = gt;
% load boundary image for each object (image)
for i = 1 : length(gt_ext)
  if (isempty(gt_ext(i).bbox))
    continue;
  end;
  % load the manual traces for the object
  num_objs = size(gt(i).bbox, 1);
  mask = cell(0, 1);
  mask_count = 0;
  while (1)
    mask_filename = [gt(i).id, '.mask.', num2str(mask_count), '.png'];
    mask_file = fullfile(manual_trace_folder, mask_filename);
    
    if (exist(mask_file, 'file'))
      im = imread(mask_file);
      im = im2bw(im, 0.5);
      mask_count = mask_count + 1;
      mask{mask_count} = im;
    else
      break;
    end;
  end;
  
  % put the mask in correct bbox index
  gt_ext(i).mask = cell(num_objs, 1);
  for m = 1 : num_objs
    bbox = gt_ext(i).bbox(m, 1:4);
    bbox = round(bbox);
    
    
    % assign the mask with most number of white pixels in this box
    max_idx = 0;
    max_count = 0;
    for jj = 1 : length(mask)
      cur_mask = mask{jj};
      submask = cur_mask(bbox(2)+1:bbox(4), bbox(1)+1:bbox(3)); % avoid idx =0 problem with matlab!
      pix_count = sum(sum(submask));
      
      if (pix_count > max_count)
        max_count = pix_count;
        max_idx = jj;
      end;
    end;
    
    if (max_idx > 0)
      gt_ext(i).mask{m} = mask{max_idx};
    else
      gt_ext(i).mask{m} = [];
    end;
  end;
end;
return;
