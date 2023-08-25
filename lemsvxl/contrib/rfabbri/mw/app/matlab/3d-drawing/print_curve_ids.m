clear all;
close all;

% addpath ~/lemsvxl/src/contrib/ntrinh/matlab/utils/
% %addpath ./amsterdam-house-quarter-size-small-subset-mcs-work/cemv/
% %addpath ../../Curve-Sketch-Datasets/amsterdam-house-quarter-size-small-subset-mcs-work/
% 
% path_to_out='./amsterdam-house-quarter-size-original-order-small-subset-mcs-work/minlength-10_mininliersview-8_minviews-4_48-conf_edgel-support/anch-10-13_epiangle-10_onlyMark_bothAnchors/';
% 
% read_curve_sketch2;
numIM = 50;
% nR = size(recs,2);

selected_curves = cell(48,1);
selected_curves{1,1} = 447;
selected_curves{2,1} = 494;
selected_curves{3,1} = 462; 
selected_curves{4,1} = 740; 
selected_curves{5,1} = 646; 
selected_curves{6,1} = 763; 
selected_curves{7,1} = 441; 
selected_curves{8,1} = 397; 
selected_curves{9,1} = 328; 
selected_curves{10,1} = 247; 
selected_curves{11,1} = 323; 
selected_curves{12,1} = 316; 
selected_curves{13,1} = [22 39 86 234 269 276 296 298 620 673 720 742 776 778]; 
selected_curves{14,1} = 813; 
selected_curves{15,1} = [258 349 501 503 524 633 761]; 
selected_curves{16,1} = 300; 
selected_curves{17,1} = 545; 
selected_curves{18,1} = 324; 
selected_curves{19,1} = [227 264]; 
selected_curves{20,1} = [307 318];
selected_curves{21,1} = [289 307 565]; 
selected_curves{22,1} = 356; 
selected_curves{23,1} = [];
selected_curves{24,1} = [331 346 397]; 
selected_curves{25,1} = [436 437 473]; 
selected_curves{26,1} = [540 543]; 
selected_curves{27,1} = 229; 
selected_curves{28,1} = [252 276]; 
selected_curves{29,1} = 258; 
selected_curves{30,1} = 296; 
selected_curves{31,1} = 261; 
selected_curves{32,1} = 403; 
selected_curves{33,1} = 411; 
selected_curves{34,1} = 588; 
selected_curves{35,1} = 760; 
selected_curves{36,1} = 765; 
selected_curves{37,1} = [587 669]; 
selected_curves{38,1} = [261 279]; 
selected_curves{39,1} = 353; 
selected_curves{40,1} = 230; 
selected_curves{41,1} = 302; 
selected_curves{42,1} = 234; 
selected_curves{43,1} = 184; 
selected_curves{44,1} = [84 98 138]; 
selected_curves{45,1} = 271; 
selected_curves{46,1} = [264 270]; 
selected_curves{47,1} = [41 222]; 
selected_curves{48,1} = [75 225 232 233]; 
idx = 1;

for n=9:9

    %if(n~=11 && n~=14)
        
        %if(size(selected_curves{idx,1},2) > 0)
    
            fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
            curP = (fscanf(fid,'%f',[4 3]))';

            [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);

            curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
            imshow(curIM);
            set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
            hold on;

            %draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,selected_curves{n,1});
            cons=draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);

            hold off;
            %print_pdf([num2str(n-1,'%08d'),'.pdf']);
            %close all;
        %end
        idx = idx + 1;
    %end
    
end