mypath='/home/rfabbri/lib/data/oxford/corridor/';

P1orig = load([mypath 'bt.000.P']);
P2orig = load([mypath 'bt.004.P']);
P1orig = P1orig/norm(P1orig);
P2orig = P2orig/norm(P2orig);

Forig = vgg_F_from_P(P1orig, P2orig);

[P1M, P2M] = fabbri_world_to_1st_cam(P1orig,P2orig);

Fnew = vgg_F_from_P(P1M, P2M);

[P1, P2]=GetPmatrixFromFmatrix(Fnew);

Fproj = vgg_F_from_P(P1, P2);

[K1, R1, T1] = vgg_KR_from_P(P1M);
P1M_qr = K1*[R1 T1];

[K2, R2, T2] = vgg_KR_from_P(P2M);
P2M_qr = K2*[R2 T2];

Fqr = vgg_F_from_P(P1M_qr, P2M_qr);


% Sanity check
disp('Original:');
Forig/norm(Forig)

disp('NEW After fabbri_world-to_1st_cam:');
Fnew/norm(Fnew)

disp('From the NEW fundamental matrix, get canonical cameras, and find fundamental matrix again:');
Fproj/norm(Fproj)

disp('From the NEW fundamental matrix, get canonical cameras, and find fundamental matrix again:');
Fqr/norm(Fqr)
