clear all;
close all;

read_curve_sketch4;

numIM = 50;
nR = size(recs,2);

selected_curves = cell(48,1);
selected_curves{1,1} = [100 389 446];
selected_curves{2,1} = [423 434 442 493];
selected_curves{3,1} = [162 168 461]; 
selected_curves{4,1} = [89 728 739]; 
selected_curves{5,1} = [74 137 645]; 
selected_curves{6,1} = [58 103 762]; 
selected_curves{7,1} = [323 440]; 
selected_curves{8,1} = [256 396]; 
selected_curves{9,1} = [225 327]; 
selected_curves{10,1} = [179 246]; 
selected_curves{11,1} = [322]; 
selected_curves{12,1} = [315]; 
selected_curves{13,1} = [405]; 
selected_curves{14,1} = [243 364]; 
selected_curves{15,1} = [384 389]; 
selected_curves{16,1} = [299 337]; 
selected_curves{17,1} = [228 295 544]; 
selected_curves{18,1} = [286 323 374]; 
selected_curves{19,1} = [226 263 471]; 
selected_curves{20,1} = [283 306 317];
selected_curves{21,1} = [40 54 288 306 564]; 
selected_curves{22,1} = [355 463]; 
selected_curves{23,1} = [374];
selected_curves{24,1} = [330 345 396 551]; 
selected_curves{25,1} = [435 436 472]; 
selected_curves{26,1} = [539 542]; 
selected_curves{27,1} = [198 216 228 231 242]; 
selected_curves{28,1} = [228 229 242 251 275 403]; 
selected_curves{29,1} = [198 257 314]; 
selected_curves{30,1} = [17 295 309]; 
selected_curves{31,1} = [260]; 
selected_curves{32,1} = [205 402]; 
selected_curves{33,1} = [220 410]; 
selected_curves{34,1} = [587 619]; 
selected_curves{35,1} = [759]; 
selected_curves{36,1} = [130 764]; 
selected_curves{37,1} = [586 668]; 
selected_curves{38,1} = [260 278 300 310]; 
selected_curves{39,1} = [271 285 352]; 
selected_curves{40,1} = [229 243]; 
selected_curves{41,1} = [301 309]; 
selected_curves{42,1} = [233 237]; 
selected_curves{43,1} = [147 183]; 
selected_curves{44,1} = [83 97 136 137]; 
selected_curves{45,1} = [52 53 270 281]; 
selected_curves{46,1} = [263 269 271 287]; 
selected_curves{47,1} = [40 221 245]; 
selected_curves{48,1} = [74 224 231 232 239]; 

idx = 1;

for n=1:numIM
    
    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';

    [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);

    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;

    if(n~=11 && n~=14)
        
        if(size(selected_curves{idx,1},2) > 0)

            draw_cons(['./curves/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,selected_curves{idx,1});
            %disp_edg(edg,0);

        end
        idx = idx + 1;
    end
    
    
    for r=1:nR

        curCurve = recs{1,r};
        numSamples = size(curCurve,1);

        reprojCurve = zeros(numSamples,2);

        for s=1:numSamples

            curSample = [(curCurve(s,:))';1];
            imSample = curP*curSample;
            imSample = imSample./imSample(3,1);

            reprojCurve(s,1) = imSample(1,1);
            reprojCurve(s,2) = imSample(2,1);

        end

        plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'-g','LineWidth',1);
    
    end
    
    hold off;
    print_pdf([num2str(n-1,'%08d'),'.pdf']);
    close all;
    
    
end