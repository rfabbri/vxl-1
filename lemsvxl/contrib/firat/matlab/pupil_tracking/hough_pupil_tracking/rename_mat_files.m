% This is rename_mat_files.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 18, 2011
% \update Mar 18, 2011

% Copyright (c) 2011 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

function rename_mat_files(mat_folder, S)
    files = dir([mat_folder '/*.mat']);
    for i = 1:length(files)
        filename = files(i).name;
        newfilename = [filename(1:end-4) '_' S '.mat'];
        movefile([mat_folder '/' filename], [mat_folder '/' newfilename]);        
    end
end
