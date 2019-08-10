%% Check 2 x 1 1/2 points SRS, following Fabbri et al. 2019
%% Special thanks to anonymous reviewer for this short demo
%% Simply runs on completely random data.

% always work from this folder
cd /home/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work

display('--------------------------------------')
display('---   demo fast p3p from OpenMVG   ---')
display('--------------------------------------')
%% Functions
v2skew =@(v) [0,-v(3),v(2);v(3),0,-v(1);-v(2),v(1),0]; % from vector to skew matrix
v2Rot  =@(v) expm(v2skew(v)); % from vector to rotation
skew2v =@(S) [S(3,2);S(1,3);S(2,1)]; % from skew matrix to vector
%% Generate data
% true rotation and translation
R_tilde = v2Rot(randn(3,1));
T_tilde = [0,0,10]'+randn(3,1);
% 3 scene points
X = randn(3,3);
Gamas = X;
x  = (R_tilde*X' + T_tilde*ones(1,3))'
% two camera points (last element = 1)
gamas = x./(x(:,3)*[1,1,1]);
gamas = gamas(:,1:2);
%% call P3P
[Rots,Transls,solve_time] = rf_p3p_root_find_function_any(gamas,Gamas);
% solve time in microseconds
%% check rotation and translation
% disp(['run time = ' num2str(rtime) ' microseconds'])
N = length(Rots);
number_of_solutions=N;
fail=1;
dRbest = -1;
dTbest = -1;
for n = 1:N
    dR = norm(skew2v(Rots{n}*R_tilde'));
    dT = norm(Transls{n}-T_tilde);
    if dR+dT < 10^-4 
%        display([num2str(n),'th solution'])
%        display(['dR =',num2str(dR)]);
%        display(['dT =',num2str(dT)]);
        %vectors_vR_vT = [calc_v_from_S(Rots{n}*R_tilde'),Transls{n}-T_tilde]
        fail=0;
        dRbest = dR;
        dTbest = dT;
    end
end
