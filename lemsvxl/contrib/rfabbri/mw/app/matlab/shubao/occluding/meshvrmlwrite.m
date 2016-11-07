function [] = meshvrmlwrite(fid,pts3d,tri)
%MESHVRMLWRITE save the point cloud into a vrml file
%
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: May, 2006


fprintf(1,'Saving the mesh as VRML ...\n');
%% write the preface of VRML
fprintf(fid, '#VRML V2.0 utf8\n');
fprintf(fid, 'DirectionalLight {\n');
fprintf(fid, '  on TRUE\n');
fprintf(fid, '  intensity 0.6\n');
fprintf(fid, '  ambientIntensity 0\n');
fprintf(fid, '  color 1.0 1.0 1.0\n');
fprintf(fid, '  direction 0.0 0.5 -1.0\n');
fprintf(fid, '}\n');
fprintf(fid, 'PointLight {\n');
fprintf(fid, '  on TRUE\n');
fprintf(fid, '  intensity 0.8\n');
fprintf(fid, '  color 1.0 1.0 1.0\n');
fprintf(fid, '  location 300.0 800.0 -200.0\n');
fprintf(fid, '}\n');
fprintf(fid, 'PointLight {\n');
fprintf(fid, '  on TRUE\n');
fprintf(fid, '  intensity 0.8\n');
fprintf(fid, '  color 1.0 1.0 1.0\n');
fprintf(fid, '  location -300.0 800.0 -200.0\n');
fprintf(fid, '}\n');
% fprintf(fid, 'SpotLight {\n');
% fprintf(fid, '  on TRUE\n');
% fprintf(fid, '  intensity 0.6\n');
% fprintf(fid, '  color 1.0 1.0 1.0\n');
% fprintf(fid, '  location 5.0 -2.0 0.0\n');
% fprintf(fid, '  direction -1.0 0.0 0.0\n');
% fprintf(fid, '  cutOffAngle 0.5\n');
% fprintf(fid, '}\n');
fprintf(fid, 'Shape{\n');
fprintf(fid, '  appearance Appearance{\n'); 
% % material copper
% fprintf(fid, '    material Material{\n');
% fprintf(fid, '      diffuseColor 0.7038 0.27048 0.0828\n');
% fprintf(fid, '      ambientIntensity 0.25\n');
% fprintf(fid, '      emissiveColor 0.0 0.0 0.0\n');
% fprintf(fid, '      specularColor 0.256777 0.137622 0.086014\n');
% fprintf(fid, '      shininess 12.8\n');
% fprintf(fid, '      transparency 0.0\n');
% fprintf(fid, '    }\n');
% % material jade
% fprintf(fid, '    material Material{\n');
% fprintf(fid, '      diffuseColor 0.54 0.89 0.63\n');
% fprintf(fid, '      ambientIntensity 0.20\n');
% fprintf(fid, '      emissiveColor 0.0 0.0 0.0\n');
% fprintf(fid, '      specularColor 0.316228 0.316228 0.316228\n');
% fprintf(fid, '      shininess 12.8\n');
% fprintf(fid, '      transparency 0.0\n');
% fprintf(fid, '    }\n'); 
% material human skin
fprintf(fid, '    material Material{\n');
fprintf(fid, '      diffuseColor 0.84 0.58 0.39\n');
fprintf(fid, '      ambientIntensity 0.50\n');
fprintf(fid, '      emissiveColor 0.0 0.0 0.0\n');
fprintf(fid, '      specularColor 0.25 0.25 0.25\n');
fprintf(fid, '      shininess 12.8\n');
fprintf(fid, '      transparency 0.0\n');
fprintf(fid, '    }\n'); 
% texture
% fprintf(fid, '    texture ImageTexture {\n');
% fprintf(fid, '      url "grass.jpg"\n');
% fprintf(fid, '      repeatS TRUE\n');
% fprintf(fid, '      repeatT TRUE\n');
% fprintf(fid, '    }\n');
fprintf(fid, '  }\n\n');
fprintf(fid, '  geometry IndexedFaceSet{\n'); 
fprintf(fid, '    solid FALSE\n'); 
fprintf(fid, '    ccw TRUE\n'); 
fprintf(fid, '    convex TRUE\n');

%% write the coordinates
fprintf(fid, '    coord Coordinate{\n');
fprintf(fid, '      point[\n');
npts3d = size(pts3d,2);
for i = 1:npts3d
    fprintf(fid, '      %d\t%d\t%d,\n', pts3d(1,i), pts3d(3,i), pts3d(2,i));
end
fprintf(fid, '      ]\n');
fprintf(fid, '    }\n');

%% write the faces
fprintf(fid, '    coordIndex[\n');
% to calculate the point index, use another array
ntri = size(tri,2);
for i = 1:ntri
    fprintf(fid, '    %d,\t%d,\t%d\t-1,\n',tri(1,i),tri(2,i),tri(3,i));
end
fprintf(fid, '    ]\n');

% write normal
% fprintf(fid, '    normalPerVertex TRUE\n');
% fprintf(fid, '    normal Normal{\n');
% fprintf(fid, '      vector [\n');
% for i = 1:npts3d
%     fprintf(fid, '      %d\t%d\t%d,\n', normal(1,i), normal(3,i), normal(2,i));
% end
% fprintf(fid, '      ]\n');
% fprintf(fid, '    }\n');
fprintf(fid, '  }\n');
fprintf(fid, '}');
