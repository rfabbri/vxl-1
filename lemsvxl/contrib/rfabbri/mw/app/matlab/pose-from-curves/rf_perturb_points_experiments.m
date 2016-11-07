clear all;

maxcount = 500;
%maxcount = 10;



for i=1:maxcount
  [Rots, Transls, gama1_pert,gama2_pert,tgt1_pert,tgt2_pert] = rf_pose_from_point_tangents_root_find_function_real();

  if ~isempty(Rots)
    break;
  end
end


%rf_real_point_tangents
id1 = 915;
id2 = 801;

rf_synthetic_point_tangent_curves

for i=1:16; norm(R_gt-Rots{i}),i,end

