function [bSol,btin,bin2,oldbrd,dim,eqns,it2,ttin,bin_first] = DimensionReductionRANSAC(m1in,m2in,NoSamples,NoParameter,pth,...
	th,subth,ttin,FcnEquations,FcnSolve,FcnInlier,NoOrigMatches)
%
%
%


nn=size(m1in,2);
% write down all equations
eval(['eqns = ', FcnEquations,'(m1in,m2in);']);
NoEqOfMatch = floor(size(eqns,1)/nn);

dim=NoParameter; 
lower=1; 
brd=nn;
while lower,
	dim=dim-1;
	fprintf('Running RANSAC for dimension %d',dim);
	p=0;
	btin=subth*nn; % check for up to subth inliers within subspace
	oldbrd=brd;
	brd=0; it2=0;
	while (p < pth & it2 < 2000)		
		% get matches
		NumSam=ceil(dim/NoEqOfMatch);
		rd = ceil(rand(1,NumSam)*nn);
		for o = 2 : NoEqOfMatch
			rd((o-1)*NumSam+1:o*NumSam)=rd(1:NumSam)+(o-1)*nn;
		end
		rd=rd(1:dim);
		
		% compute solution
		clear CurrSol;
		CurrSol = eval([FcnSolve,'(eqns(rd,:),NoParameter-dim+1);']);
		% get inliers
		[tin,in] = eval([FcnInlier,'(CurrSol,m1in,m2in,th);']);
		
		if tin > btin	
% 			otin=tin; tin=0;
%             first = 1;
% 			while not(tin>=otin) & (first == 1 | tin > dim) & first < 30
% 				if first ~= 1
% 					otin=tin;
% 				end
% 				%refine solution
% 				ineq = in;
% 				for o = 2 : NoEqOfMatch
% 					ineq = [ineq; in];
% 				end
% 				beq=diag(ineq)*eqns; % equations coming from all inliers in subspace
% 				% compute solution
% 				clear CurrSol;
% 				CurrSol = eval([FcnSolve,'(beq,NoParameter-dim+1,1);']);
% 				% get inliers
% 				[tin,in] = eval([FcnInlier,'(CurrSol,m1in,m2in,th);']);
%                 first = first + 1;
%             end
            if tin > btin
				btin = tin;
				bSol = CurrSol;
				brd= rd;
				bin2= in;
			end
		end
		
		% update iteration parameters
        it2=it2+1;
%         p = 1-(1-(btin/nn)^dim)^it2;
       %if ( it2 > 500)
		 p = 1-(1-(btin/NoOrigMatches)^dim)^it2;
	   %end
	end
	
	
    ttin(dim)=btin;

	if (dim == NoParameter-1 && btin > subth*nn)
		[tin,in] = eval([FcnInlier,'(bSol,m1in,m2in,3*th);']);
		fprintf(' %d weaken inlier for first dimension reduction\n',tin)
		bin_first=in;
	end
	
% 	fprintf('DEBUG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n');
% 	if (dim == 8)
% 			bF6=bSol;
% 			bSol6=bSol;
% 			brd6= brd;
% 			bin6= bin2;
% 	end
% 	
	if (dim == NoParameter-1 && btin == subth*nn)
		bin2 = zeros(size(m1in,2),1);
		btin = sum(tin);
		bSol = CurrSol;
		brd = rd;
		bin2 = 0;
		bin_first = in;
		lower = 0;
	end
	fprintf(' after %d iterations with %d inliers after refinement\n',it2,btin);
    
	if not(brd),
        lower=0;
    end
    if dim==1,
        lower=0;
    end
	end

% 	bSol=bF6;
% 	bSol=bSol6;
% 	brd= brd6;
% 	bin2= bin6;
% 	dim = 7;
btin = sum(bin2); 