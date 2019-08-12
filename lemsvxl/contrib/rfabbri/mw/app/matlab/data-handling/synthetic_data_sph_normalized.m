% this will convert to normalized image coords by inverting K
% after / inside running synthetic_data_sph. 

pts = zeros(npts,3,nviews);
zeros(size(pts_img));  % normalized coords
tgts = zeros(npts,3,nviews) % normalized coords
for v=1:nviews
  % mex function synthetic_data:
  pts1 = pts_img(:,:,v);
  tgts1 = tgts_img(:,:,v);
  R1 = R(:,:,v)
  C1 = C(:,v)
  T1 = -R1*C1;
  Kinv = inv(K);

  % convert to normalized image coordinates
  tgt1_2d_pt = tgts1 + pts1(:,1:2);
  pts1 = Kinv*[pts1 ones(size(pts1,1),1)]';
  pts1 = pts1';
  pts1(:,1) = pts1(:,1)./pts1(:,3);
  pts1(:,2) = pts1(:,2)./pts1(:,3);
  pts1(:,3) = 1;

  tgt1_2d_pt_normal = Kinv*[tgt1_2d_pt ones(size(tgt1_2d_pt,1),1)]';
  tgt1_2d_pt_normal = tgt1_2d_pt_normal';
  tgt1_2d_pt_normal(:,1) = tgt1_2d_pt_normal(:,1) ./ tgt1_2d_pt_normal(:,3);
  tgt1_2d_pt_normal(:,2) = tgt1_2d_pt_normal(:,2) ./ tgt1_2d_pt_normal(:,3);
  tgt1_2d_pt_normal(:,3) = tgt1_2d_pt_normal(:,3) ./ tgt1_2d_pt_normal(:,3);

  tgts1 = tgt1_2d_pt_normal - pts1;

  ssum = sqrt(sum(tgts1.*tgts1, 2));
  norms =  [ssum ssum ssum];
  tgts1 = tgts1 ./ norms;
  pts(:,:,v) = pts1; 
  tgts(:,:,v) = tgts1;
end
