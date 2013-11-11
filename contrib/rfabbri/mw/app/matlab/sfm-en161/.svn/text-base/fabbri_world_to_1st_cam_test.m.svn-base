% Tests fabbri_world_to_1st_cam

mypath = '/home/rfabbri/lib/data/oxford/corridor/';

P1 = load([mypath 'bt.000.P']);
P2 = load([mypath 'bt.004.P']);

F = vgg_F_from_P(P1,P2);

[P1_out, P2_out] = fabbri_world_to_1st_cam(P1,P2);


F_out = vgg_F_from_P(P1_out,P2_out);

F = F/norm(F);
F_out = F_out/norm(F_out);

F,F_out

[e1,e2] = epipole(F);
e1 = e1/e1(3)
e2 = e2/e2(3)

[e1_out,e2_out] = epipole(F_out);
e1_out = e1_out/e1_out(3)
e2_out = e2_out/e2_out(3)


