% This script creates screenshots of curve fragment graph for ETHZ dataset
% (c) Nhon Trinh
% Date: Nov 17, 2008

clear all;
close all;

%% Input


jpg_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\reorganized-data\all_originals';
cem_folder = 'D:\vision\data\ethz-cfg\cfg2';
cfg_screenshot_cmd = 'D:\vision\projects\lemsvxl\build32-vs05\brcv\seg\dbdet\examples\debug\screenshot_cfg_command.exe';
screenshot_folder = 'D:\vision\data\ethz-cfg\cfg2-screenshot';

%% Create screenshots

dirlist = dir(jpg_folder);
for i = 1 : length(dirlist)
  if (dirlist(i).isdir)
    continue;
  end;
  
  % jpg filename
  jpg_filename = dirlist(i).name;
  jpg_file = fullfile(jpg_folder, jpg_filename);
  
  % cem filename
  [pathstr, name, ext, versn] = fileparts(jpg_filename);
  cem_filename = [name, '.cem'];
  cem_file = fullfile(cem_folder, cem_filename);
  
  % screenshot filename
  screenshot_filename = [cem_filename, '.png'];
  screenshot_file = fullfile(screenshot_folder, screenshot_filename);
  
  % print out info
  fprintf(1, '\njpg_file =%s\n', jpg_file);
  fprintf(1, 'cem_file =%s\n', cem_file);
  fprintf(1, 'screenshot_file =%s\n', screenshot_file);
  
  % dos-command
  dos_command = [cfg_screenshot_cmd, ' ', jpg_file, ' ', cem_file, ...
      ' ', screenshot_file];
  system(dos_command);
  
end;
