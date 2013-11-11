resultpath = '../result/zebrahorse_edge30/';
debugpath = '../debug/zebrahorse_edge30/';
fname_pwn = 'pointcloudrs_nointerp.pwn';
fname_vrml = 'pointcloudrs_nointerp.wrl';


load([debugpath 'programdata.mat']);

cellnum = length(pts3dcell);
pts3dcell_nointerp = cell(cellnum);
for i = 1:cellnum
    pts3dcell_nointerp{i} = pts3dcell{i}(:,:,1);
end

pts3dcellrs_nointerp = cellresample(pts3dcell_nointerp,5);
pts3drs_nointerp = cell2matrix(pts3dcellrs_nointerp);


fid_pwn = fopen([resultpath fname_pwn], 'w+');
pcpwnwrite(fid_pwn, pts3drs_nointerp);
fclose(fid_pwn);

fid_vrml = fopen([resultpath fname_vrml], 'w+');
pcvrmlwrite(fid_vrml, pts3drs_nointerp);
fclose(fid_vrml);
