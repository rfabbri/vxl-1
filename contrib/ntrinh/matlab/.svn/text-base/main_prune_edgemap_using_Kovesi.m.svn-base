% This script cleans up Pb edges and re-estimate edge orientation
% It does so by linking the edges and compute edge orientation using
% neighboring edges in the linked contours
% Date: Feb 4, 2009

%%
clear all;
close all;

%% input data
objectlist_file = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\objectlist.txt';

orig_pb_edgemap_folder  = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_pb_edges_pyramid';
pb_edgemap_extension    = '_pb_edges.png';
pb_edgeorient_extension = '_pb_orient.txt';
edge_threshold = 15;
link_edge_length_threshold = 4;
max_number_pyramid_levels = 25;

clean_edgemap_folder = 'D:\vision\projects\symseg\xshock\all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4';
% clean_edgeorient_folder; 
% clean_cemv_folder;
cemv_extension = '_pb_linked.cem';

%% Process

fprintf(1, '\n\nCleaning Pb edgemaps using Kovesi edge linker\n');

fprintf(1, 'objectlist_file= "%s"\n', objectlist_file);
fprintf(1, 'orig_pb_edgemap_folder= "%s"\n', orig_pb_edgemap_folder);
fprintf(1, 'pb_edgemap_extension= "%s"\n', pb_edgemap_extension);
fprintf(1, 'pb_edgeorient_extension= "%s"\n', pb_edgeorient_extension);
fprintf(1, 'cemv_extension= "%s"\n', cemv_extension);

fprintf(1, 'edge_threshold= %g\n', edge_threshold);
fprintf(1, 'link_edge_length_threshold= %g\n', link_edge_length_threshold);
fprintf(1, 'max_number_pyramid_levels = "%d"\n', max_number_pyramid_levels);

fprintf(1, 'clean_edgemap_folder= "%s"\n', clean_edgemap_folder);
fprintf(1, '\n');

% load objectlist
fid = fopen(objectlist_file, 'r');
C = textscan(fid, '%s');
fclose(fid);
objectlist = C{1};

% iterate thru the edgemap files
for i = 1 : length(objectlist)
  objectname = char(objectlist{i});
  
%    if (~strcmp(objectname, 'giraffes_dragon'))
%      continue;
%    end;
  
  fprintf(1, '\nObject name = %s\n', objectname);
  input_folder = fullfile(orig_pb_edgemap_folder, objectname);
  
  if (~exist(input_folder, 'dir'))
    fprintf(1, '  ERROR: could not find edgemap folder for object = %s \n', objectname);
    continue;
  end;
  
  % create a folder for pruned edgemap files
  output_folder = fullfile(clean_edgemap_folder, objectname);
  if (~exist(output_folder, 'dir'))
    mkdir(output_folder);
  end;
    
  % iterate thru list of edgemap files
  for kk = 0 : max_number_pyramid_levels
    base_name = [objectname, '_', num2str(kk, '%02d')];
    fprintf(1, '\n  base_name = %s\n', base_name);
    edgemap_filename = [base_name, pb_edgemap_extension];
    edgemap_file = fullfile(input_folder, edgemap_filename);
    
    % not all images have all scales
    if (~exist(edgemap_file, 'file'))
      fprintf(1, '  Edgemap does not exist. Stopped.\n');
      break;
    end;
    
    % load the edgemap ----------------------------------------------------
    
    % Check to see whether this file has been processed.
    clean_edgemap_file = fullfile(output_folder, edgemap_filename);
    if (exist(clean_edgemap_file, 'file'))
      fprintf(1, '  Edgemap has been processed. Skipped.\n');
      continue;
    end;
    
    im = imread(edgemap_file);
    edgemap = im > edge_threshold;
    % link the edges using kovesi's linker
    [edgelist, labelededgeim] = edgelink(edgemap, link_edge_length_threshold);
      
    % form a new edge map from the edgelist
    clean_edgemap = uint8(zeros(size(edgemap)));
    for m = 1 : length(edgelist)
      edge = edgelist{m};
      for p = 1 : size(edge, 1)
        clean_edgemap(edge(p, 1), edge(p, 2)) = 255;
      end;
    end;
    
    
    
    mask = (clean_edgemap > 0);

    fprintf(1, '  Number of edges-initial = %d\n', sum(sum(edgemap)));
    fprintf(1, '  Number of edges-after linking = %d\n', sum(sum(mask)));
    fprintf(1, '  Number of edges-removed = %d\n', sum(sum(edgemap-mask)));
    
    % load edge orientation -----------------------------------------------
    edgeorient_filename = [base_name, pb_edgeorient_extension];
    edgeorient_file = fullfile(input_folder, edgeorient_filename);
    edgeorient = dlmread(edgeorient_file, ' ');

    % remove orientation edge locations with no edges
    clean_edgeorient = edgeorient .* (clean_edgemap>0);

    % write clean edgeorient file
    clean_edgeorient_file = fullfile(output_folder, edgeorient_filename);
    
    % Prepare to save the contour edgemap to a file -----------------------
  
    % augment the edgelist with its orientation
    for m = 1 : length(edgelist)
      contour = edgelist{m};
      pix_r = contour(:, 1);
      pix_c = contour(:, 2);

      pix_orient = clean_edgeorient(sub2ind(size(edgemap), pix_r, pix_c));
      edgelist{m} = [contour, pix_orient];
    end;
  
    clean_cemv_filename = [base_name, cemv_extension];
    cemv_file = fullfile(output_folder, clean_cemv_filename);
  
    % Write all files out -------------------------------------------------
    if (1)
      imwrite(clean_edgemap, clean_edgemap_file);
      dlmwrite(clean_edgeorient_file, clean_edgeorient, ' ');
      det_save_cemv(cemv_file, edgelist);
    end;



  end;
  

  
  
  
  
  
  
  
  
  
  
  
  %
  if (0)
    gx = cos(edgeorient) .* mask;
    gy = sin(edgeorient) .* mask;
  
    [X, Y] = meshgrid(1:size(im, 2), 1:size(im, 1));
    X = X(:);
    Y = Y(:);

    % edges to display
    idx = find(mask > 0);

    gx = gx(idx);
    gy = gy(idx);

    X = X(idx);
    Y = Y(idx);

    % display the edgemap
    figure(1);
    imshow(edgemap);
    hold on;
    drawedgelist(edgelist, size(im), 1, 'rand', 1);
  %  quiver(X, Y, gx, gy);
    hold off;
  end;
end;

