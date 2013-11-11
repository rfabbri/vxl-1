% Author: Jason Mallios
% Purpose: Displays a sequence of colormapped images in a directory.
% IMPORTANT: The directory must consist of image pairs.

function displayMapDirectory(dir_name)

% GET FILES IN DIRECTORY
dir_files = dir(dir_name);

for i=1:2:size(dir_files,1)
    dir_file = dir_files(i).name;
    last_letter = dir_file(size(dir_file,2));
    if (last_letter=='f')
        displayMapImage([dir_name '/' dir_file], [dir_name '/' dir_files(i+1).name]);
        pause(0.01);
    end
end

pause();
close all;

end