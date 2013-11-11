  pts0_r = zeros(size(pts0));
  pts0_r(:,1) = -pts0(:,2);
  pts0_r(:,2) = pts0(:,1);
  pts0 = pts0_r;

  pts1_r = zeros(size(pts1));
  pts1_r(:,1) = -pts1(:,2);
  pts1_r(:,2) = pts1(:,1);
  pts1 = pts1_r;

  ap0_r = zeros(size(ap0));
  ap0_r(:,1) = -ap0(:,2);
  ap0_r(:,2) = ap0(:,1);
  ap0 = ap0_r;

  ap1_r = zeros(size(ap1));
  ap1_r(:,1) = -ap1(:,2);
  ap1_r(:,2) = ap1(:,1);
  ap1 = ap1_r;

  box_r = box;
  box_r(:,1) = -box(:,2);
  box_r(:,2) = box(:,1);
  box = box_r;

  if max(size(e0)) ~= 0
    % Rotate epipoles
    e0_r = zeros(size(e0));
    e0_r(:,1) = -e0(:,2);
    e0_r(:,2) = e0(:,1);
    e0 = e0_r;

    e1_r = zeros(size(e1));
    e1_r(:,1) = -e1(:,2);
    e1_r(:,2) = e1(:,1);
    e1 = e1_r;
  end
