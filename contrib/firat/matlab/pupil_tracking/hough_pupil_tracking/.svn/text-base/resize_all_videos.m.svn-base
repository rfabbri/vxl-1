% This is resize_all_videos.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 15, 2011
% \update Mar 22, 2011

% Copyright (c) 2011 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

function resize_all_videos(video_folder, mat_folder, S)
    files = dir([video_folder '/*.avi']);    
    for i = 1:length(files)
        video_name = get_objectname_from_filename(files(i).name)
        mat_file = [mat_folder '/' video_name '_' num2str(S) '.mat'];
        if ~exist(mat_file, 'file')
            video_file = [video_folder '/' video_name '.avi'];
            mov = read_video(video_file, S, S); 
            save('-v7.3', mat_file, 'mov');
            clear mov;
        end
    end
end

function objname = get_objectname_from_filename(filename, ext1)
        slashpos = strfind(filename, '/');
        if length(slashpos) > 0
            slashpos = slashpos(end);
            filename = filename(slashpos + 1 : end);
        end
        if(exist('ext1', 'var'))
            LF = length(filename);
            LE = length(ext1);
            ext2 = filename(LF - LE + 1 : LF);
            if strcmp(ext1, ext2)
                objname = filename(1 : LF - LE - 1);
            else
                objname = filename;
            end  
        else
            dotpos = strfind(filename, '.');
            dotpos = dotpos(end);
            objname = filename(1 : dotpos - 1);
        end
               
end
