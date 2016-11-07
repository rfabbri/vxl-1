% show slice countours and also 3D plots


nz = size(phi2,3);
for i=1:nz
  showseg(i,img,phi,phi2);
end
