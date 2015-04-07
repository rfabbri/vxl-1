function  [bSol,m1in1,m2in1,m1in2,m2in2,m1in,m2in,bSol1,bin,bin2,bin3,it,it2,it3,m1out2,m2out2,ttin,ttin2] = ...
	QDEGSAC(m1,m2,th,pth,rho,NoParameters,NoSamples,FcnEquations,FcnSolve,FcnInlier)
%
% QDEGSAC determines the "robust nullspace" of the estimation problem fromaccording to:
% J.-M. Frahm, M. Pollefeys, “RANSAC for (Quasi-)Degenereate data % (QDEGSAC)”, Proc. CVPR, 2006
%
% It reduces the number of constraints by one and looks at the change in the
% number of inliers. If the number of inliers is still higher than 50% of
% the number of inliers of the solution for full number of samples the
% number of samples is reduced by one again. 
% After getting to few inliers compared to the best solution with the
% full number of samples it uses the outlier of that solution to compute a
% solution. Using the outlier of that solution gives the advantage of
% finding a solution that is not computed from degenerated data.
%
% call:
%   [bSol,m1in1,m2in1,m1in2,m2in2,m1in,m2in,bSol1,bin,bin2,bin3,it,it2,it3,m1out2,m2out2,ttin,ttin2] = ...
%	QDEGSAC(m1,m2,th,pth,rho,NoParameters,NoSamples,FcnEquations,FcnSolve,FcnInlier)
% m1, m2 contain the corresponding points m1(:,i) corresponds to m2(:,i)
% th             inlier threshold used in FcnInlier
% rho            percentage of inliers to stil accept the solution during
%                reducing the number of constraints
% NoParameters   number of parameters of the relation to be solved for
% NoSamples      number of samples needed get a unique solution for the relation
% FcnEquations   String for function to compute equation. This function 
%                gets the correspondences m1,m2 and returns an equation system
% FcnSolve       String for function to compute solution for equations. In 
%                dependence of the number of constraints it returns the full 
%                nullspace of solutions.
% FcnInlier      String for the inlier computation functions. This evaluates the
%                possible solutions and computes the inliers for all of
%                them. 

% init RANASAC paramters
subth = rho; % percentage needing to remain with bigger nullspace
refine = 1; % use refinement

%init data structs
in = 0; % initial inlier count
n=size(m1,2); % number of potential matches
e=zeros(n,1); % vector that will contain symmetric error  
btin=0; %initialize with no inliers

% generate the equations for all potential correspondences
eq = eval([FcnEquations,'(m1,m2);']);
NoEqOfMatch = round(size(eq,1)/n);

it=0; p=0; selindex = 1;
while p < pth,
	
	% get random sample
	rd = ceil(rand(1,NoSamples)*n);
	for o = 2 : NoEqOfMatch
		rd((o-1)*NoSamples+1:o*NoSamples)=rd(1:NoSamples)+(o-1)*n;
	end
	
	% compute solution
	clear CurrSol;
	CurrSol = eval([FcnSolve,'(eq(rd,:),1);']);
	% get inliers
	[tin,in] = eval([FcnInlier,'(CurrSol,m1,m2,th);']);
	
	if tin > btin
		if (refine)
			first = 1;
			otin=tin; tin=0;
			while not(tin>=otin)
				if first == 1
					first = 0;
				else
					otin=tin;
				end
				% refine solution
				ineq = in;
				for o = 2 : NoEqOfMatch
					ineq = [ineq; in];
				end
				beq=diag(ineq)*eq; % equations coming from all inliers in subspace
				
				% compute solution
				clear CurrSol;
				CurrSol = eval([FcnSolve,'(beq,1,1);']);
				% get inliers
				[tin,in] = eval([FcnInlier,'(CurrSol,m1,m2,th);']);
			end
		end
		if tin > btin
			btin = tin;
			bSol = CurrSol;
			brd= rd;
			bin= in;
		end
	end
	% log number of selections for each potential match
	logInlierSelection(1:size(in,1),selindex) = in;
	selindex = selindex + 1;
	
	% increment variables and probability
	it=it+1;
	%p = 1-(1-(btin/n)^8)^it;
	p = 1-(1-(btin/n)^NoParameters)^it;
	%p = 1-((1-(0.0006))^it);
end
fprintf('standard RANSAC has %d inlier and needed %d trials\n',btin,it)

ttin=zeros(1,NoSamples);
ttin(NoParameters)=btin;

bSol1=bSol;
btin1 = btin;
bin1 =bin;

m1in=m1(:,find(bin));
m2in=m2(:,find(bin));
m1out=m1(:,find(~bin));
m2out=m2(:,find(~bin));
nn=size(m1in,2);

% find correct dimension of the samples
[bSol,btin,bin2,oldbrd,dim,eqns,it2,ttin,bin_first] = DimensionReductionRANSAC(m1in,m2in,NoSamples,NoParameters,...
	pth,th,subth,ttin,FcnEquations,FcnSolve,FcnInlier,n);

fprintf('dimension reduction RANSAC has %d inlier in dimension %d\n',btin,dim+1)

if not(dim==0)
	ttin(dim)=0;
	dim=dim+1; % dim was decremented once too much
	brd = oldbrd; % best sample in dimension
	codim = NoParameters-dim;
	
	m1out2=m1in(:,find(~bin2));
	m2out2=m2in(:,find(~bin2));
	m1in1=m1in(:,find(bin2));
	m2in1=m2in(:,find(bin2));
	m1out=[m1out m1out2];
	m2out=[m2out m2out2];
	
	m1out_old = m1out;%m1in(:,find(~bin_first));
	m2out_old = m2out;%m2in(:,find(~bin_first));
	nout=size(m1out_old,2);
	%eqns=eval([FcnEquations,'(m1out,m2out);']);
	
	%seperate nearly inlier
	[wtin,win] = eval([FcnInlier,'(bSol,m1out,m2out,th);']);
	fprintf('sorted out %d weak innliers\n',wtin);
	m1out = m1out(:,find(~win));
	m2out = m2out(:,find(~win));
	nout=size(m1out,2);
	
	if (dim <NoParameters)
		% equations coming from all inliers in subspace
		ineq2 = bin2;
		%ineq2 = win;
		for o = 2 : NoEqOfMatch
			ineq2 = [ineq2; bin2];
			%ineq2 = [ineq2; win];
		end
		allbeq = diag(ineq2)*eqns;
		% closest rank approximation
		[u,s,v]=svd(allbeq);
		diag(s)';
		s(1:NoParameters,1:NoParameters)=s(1:NoParameters,1:NoParameters).*diag([1:NoParameters]<=dim);
		diag(s)';
		allbeq=s*v';
		beq =allbeq;
	
		% all equations of the outliers
		eqout = eval([FcnEquations,'(m1out,m2out);']);
		
		p=0; it3=0;  btin=0;
		while p < pth & it3 < 3000,
			NoRemSam = ceil(codim/NoEqOfMatch);
			rd = ceil(rand(1,NoRemSam)*nout);
			for o = 2 : NoEqOfMatch
				rd((o-1)*NoRemSam+1:o*NoRemSam)=rd(1:NoRemSam)+(o-1)*nout;
			end
			
			% compute solution
			clear CurrSol;
			CurrSol = eval([FcnSolve,'([beq; eqout(rd,:)],1);']);
			% get inliers
			[tin,in] = eval([FcnInlier,'(CurrSol,m1out,m2out,th);']);
			% refine
			if tin > btin
				if (refine)
					otin=tin; tin=0;
					in_old = in;
					first =1;
					while not(tin>=otin) & (tin > NoParameters | first)
						if not(first)
							otin=tin;
						else
							first = 0;
						end
						%refine solution
						ineq = in;
						for o = 2 : NoEqOfMatch
							ineq = [ineq; in_old];
						end
						reqs=[beq; diag(ineq)*eqout];
						% compute solution
						clear CurrSol;
						CurrSol = eval([FcnSolve,'(reqs,1,1);']);
						% get inliers
						[tin,in] = eval([FcnInlier,'(CurrSol,m1out,m2out,th);']);
					end
				end
				
				if (tin > btin)
					btin = tin
					bSol = CurrSol;
					brd= rd;
					bin3= in;
				end
			end
			it3=it3+1;
			
			p = 1-(1-(btin/nout)^codim)^it3;
			%p = 1-(1-(btin/n)^codim)^it3;
		end
		fprintf('Improving RANSAC has %d additional inliers on reduced set and needed %d trials\n',btin,it3)
		
		
		if (refine)
			% full set of outliers of first
			eqout = eval([FcnEquations,'(m1out_old,m2out_old);']);
			[btin,bin3] = eval([FcnInlier,'(bSol,m1out_old,m2out_old,th);']);
			%eqout = eval([FcnEquations,'(m1,m2);']);
			%[btin,bin3] = eval([FcnInlier,'(bSol,m1,m2,th);']);
			otin=btin; tin=0;
			in = bin3;
			first =1;
			while not(tin>=otin) & (tin > NoParameters | first)
				if not(first)
					otin=tin;
				else
					first = 0;
				end
				%refine solution
				ineq = in;
				for o = 2 : NoEqOfMatch
					ineq = [ineq; in];
				end
				reqs=[beq; diag(ineq)*eqout];
				% compute solution
				clear CurrSol;
				CurrSol = eval([FcnSolve,'(reqs,1,1);']);
				% get inliers
				[tin,in] = eval([FcnInlier,'(CurrSol,m1out_old,m2out_old,th);']);
			end
			if(tin > btin)
				btin = tin;
				bSol = CurrSol;
				bin3= in;
			end
		end
		fprintf('found %d additional inliers in refinement on full set\n',btin)
		m1in2=[];m2in2=[];
		[tin3,in3] = eval([FcnInlier,'(bSol,m1out_old,m2out_old,th);']);
		for i=1:size(m1out_old,2),
			if in3(i),
				m1in2=[m1in2 m1out_old(:,i)];
				m2in2=[m2in2 m2out_old(:,i)];
			end
		end
		
	else
		fprintf('No third RANSAC required\n')
		btin = 0;
		bSol = bSol1;
		bin3 = in;
		it3=0;
		m1out2=0;
		m2out2=0;
		
		m1in2=[];m2in2=[];
		for i=1:size(bin3,1),
			if bin3(i),
				m1in2=[ m1(:,i)];
				m2in2=[ m2(:,i)];
			end
		end
	end
end

m1in2=[];m2in2=[];
[tin3,in3] = eval([FcnInlier,'(bSol,m1out_old,m2out_old,th);']);
for i=1:size(m1out_old,2),
	if in3(i),
		m1in2=[m1in2 m1out_old(:,i)];
		m2in2=[m2in2 m2out_old(:,i)];
	end
end
%find out the really addtional matches
PrevKnownMatches=[];
for i = 1 : size(m1in2,2)
	for j = 1 : size(m1in,2)
		PrevKnownMatches(i,j) = (sum(m1in(:,j) == m1in2(:,i)));
	end;
end
bsg=(sum(PrevKnownMatches') ==0);
sum(bsg)

[dtin,din] = eval([FcnInlier,'(bSol,m1,m2,th);']);
if(sum(bsg) < ceil((NoParameters-dim)/NoEqOfMatch) )%| sum(bin) > dtin)
	fprintf('First RANSAC was best\n')
	bSol = bSol1;
	btin = btin1;
	bin = bin1;
end



% count inliers using also weak inliers
[ctin,cin] = eval([FcnInlier,'(bSol,m1out_old,m2out_old,th);']);
ttin2=zeros(1,NoParameters); ttin2(dim)=ctin;


figure(2)
bar([ttin;ttin2]','stacked')
