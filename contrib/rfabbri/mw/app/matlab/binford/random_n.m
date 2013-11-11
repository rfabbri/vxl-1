% pick a random unit vector in 3D normal to T
function N=random_n(T)
    V = randn(3,1);
    V(3) = -(V(1)*T(1) + V(2)*T(2)) / T(3);

    V = V/norm(V);

    teta = 2*pi*rand();
    N = cos(teta)*V + sin(teta)*cross(T,V);
