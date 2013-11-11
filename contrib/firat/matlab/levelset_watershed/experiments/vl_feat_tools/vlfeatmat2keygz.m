% This is vlfeatmat2keygz.m.

% Created on: Apr 11, 2013
%     Author: firat

function vlfeatmat2keygz(mat_folder, keygz_folder)
    files = dir([mat_folder '/*.mat']);
    if ~exist(keygz_folder,'dir')
        mkdir(keygz_folder);
    end
    for i = 1:length(files)
        X = load([mat_folder '/' files(i).name]);
        dotpos = find(files(i).name == '.', 1);
        basename = files(i).name(1:dotpos);
        keygzname = [basename 'key'];
        vl_ubcwrite(X.reg_f, X.reg_d, [keygz_folder '/' keygzname], true); 
    end
end

