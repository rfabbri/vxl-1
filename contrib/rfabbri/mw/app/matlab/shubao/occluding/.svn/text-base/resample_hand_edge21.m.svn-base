resultpath = '../result/hand_edge21/';
debugpath = '../debug/hand_edge21/';
fname_p3d = 'pointcloudrs.p3d';
fname_vrml = 'pointcloudrs.wrl';

load([debugpath 'programdata.mat']);
pts3dcellrs = cellresample(pts3dcell,10);
pts3drs = cell2matrix(pts3dcellrs);

fid_p3d = fopen([resultpath fname_p3d], 'w+');
pcp3dwrite(fid_p3d, pts3drs);
fclose(fid_p3d);

fid_vrml = fopen([resultpath fname_vrml], 'w+');
pcvrmlwrite(fid_vrml, pts3drs);
fclose(fid_vrml);
