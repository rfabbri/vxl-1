% Author: Jason Mallios
% Purpose: Displays a directory of non-colormapped images.  Only displays
%          the first image.
% IMPORTANT: The directory must consist of image pairs.

function displayNoMapDirectory(dir_name)

% GET FILES IN DIRECTORY
dir_files = dir(dir_name);

for i=1:2:size(dir_files,1)
    dir_file = dir_files(i).name;
    last_letter = dir_file(size(dir_file,2));
    if (last_letter=='f')
        displayNoMapImage([dir_name '/' dir_file]);
        pause(0.01);
    end
end

pause();
close all;

end