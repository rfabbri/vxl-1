% generates isosurface and plots it with lighting
function myiso(D,t,color)
clf
% v: 3D image
% t: isovalue
if nargin == 2
   color=[1,0.2,0.2];
end

% smooth the surface
%Ds = smooth3(D);
% Choosing an isovalue (t), extracting the isosurface and displaying it
hiso = patch(isosurface(D,t),'FaceColor',color,'EdgeColor','none');

view(45,30);axis tight 

set(hiso,'SpecularColorReflectance',0,'SpecularExponent',50);
lightangle(45,30);set(gcf,'Renderer','zbuffer'); lighting phong
