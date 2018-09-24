// to be included in synthdata_trifocal.sce

T__1 = R_1*T_w_vec;
T__2 = R_21*T__1;
T__3 = R_31*T__1;

// Starting here we treat the 2D vectors as 3D vectors
// Apply the inverse K matrix!
t__1_vec_img = [t__1_vec_img; zeros(1,size(t__1_vec_img,2))]
t__2_vec_img = [t__2_vec_img; zeros(1,size(t__2_vec_img,2))]
t__3_vec_img = [t__3_vec_img; zeros(1,size(t__3_vec_img,2))]

// gama__1_vec = inv(K)*gama__1_vec;
t__1_vec = K\t__1_vec_img;
t__2_vec = K\t__2_vec_img;
t__3_vec = K\t__3_vec_img;

tmpnorm = sqrt(sum(t__1_vec.*t__1_vec,1));
t__1_vec = t__1_vec ./ [tmpnorm;tmpnorm;tmpnorm;];
tmpnorm = sqrt(sum(t__2_vec.*t__2_vec,1));
t__2_vec = t__2_vec ./ [tmpnorm;tmpnorm;tmpnorm;];
tmpnorm = sqrt(sum(t__3_vec.*t__3_vec,1));
t__3_vec = t__3_vec ./ [tmpnorm;tmpnorm;tmpnorm;];


// Sanity check: projected tangents must match


// 3D parametrization relative to view 1 (ie, for g__1 = 1)
// From big notes book draft (6.2.3):

g__2 = zeros(size(point_ids));
g__3 = zeros(size(point_ids));
depth_gradient__1 = zeros(size(point_ids));
depth_gradient__2 = zeros(size(point_ids));
depth_gradient__3 = zeros(size(point_ids));
t__1_proj_vec = zeros(3,size(point_ids,'*'));
for i=point_ids
  n_w_1 =norm(T__1(:,i) - T__1(3,i)*gama__1_vec(:,i));
  G = depth__1(i) / n_w_1;

  g__2(i) = G*norm(T__2(:,i) - T__2(3,i)*gama__2_vec(:,i))/depth__2(i);
  g__3(i) = G*norm(T__3(:,i) - T__3(3,i)*gama__3_vec(:,i))/depth__3(i);
  depth_gradient__1(i) = G *T__1(3,i);
  depth_gradient__2(i) = G *T__2(3,i);
  depth_gradient__3(i) = G *T__3(3,i);
  
  // TANGENT EQS
  depth_gradient__2(i)*gama__2_vec(:,i) + depth__2(i)*g__2(i)*t__2_vec(:,i) - R_21*(depth_gradient__1(i)*gama__1_vec(:,i) + depth__1(i)*t__1_vec(:,i))

  // Just sanity-checking that tangents project to given tangents
  t__1_proj_vec(:,i) = T__1(:,i) - T__1(3,i)*gama__1_vec(:,i);
  t__1_proj_vec(:,i) = t__1_proj_vec(:,i) / norm(t__1_proj_vec(:,i));
  disp 'tangent projection'
  t__1_vec(:,i) - t__1_proj_vec(:,i)
end


// TODO: compute area in each image and in 3D of triangle, to guarantee
// nondegeneracy


