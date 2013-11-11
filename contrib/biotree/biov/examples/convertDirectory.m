% Author: Jason Mallios
% Purpose: Converts a directory of 16-bit indexed tiff images into
%          non-colormapped 8-bit tiff images.
% IMPORTANT: Don't run this on the original data directory or a read only
%            directory!  Make a write-able copy.  This script deletes files
%            that biov_main doesn't need.  This script assumes all images
%            ending with 00.tif are flourescent data and keeps them.

function convertDirectory(dir_name)

% GET FILES IN DIRECTORY
dir_files = dir(dir_name);
max_value = 0;

% GET GLOBAL MAXIMUM VALUE
for i=1:size(dir_files,1)
    dir_file = dir_files(i).name;
    if (size(dir_file,2)>6 ...
            && all(dir_file(size(dir_file,2)-5:size(dir_file,2)) == '00.tif'))
        im = mapToNoMap([dir_name '/' dir_file]);
        max_value = max(max_value,max(max(im(:,:,2))));
    end
end

% CONVERT GREEN CHANNEL TIFS AND DELETE UNUSED FILES
for i=1:size(dir_files,1)
    header = [num2str(i) ' OF ' num2str(size(dir_files,1))];
    dir_file = dir_files(i).name;
    if (size(dir_file,2)>6 ...
            && all(dir_file(size(dir_file,2)-5:size(dir_file,2)) == '00.tif'))
        disp([header ' CONVERTING ' dir_file]);
        im = mapToNoMap([dir_name '/' dir_file]);
        im = im / max_value;
        imwrite(im, [dir_name '/' dir_file], 'tif');
    elseif (size(dir_file,2)>2)
        disp([header ' DELETING ' dir_file]);
        delete([dir_name dir_file]);
    else
        disp([header ' IGNORING ' dir_file]);
    end
end

end