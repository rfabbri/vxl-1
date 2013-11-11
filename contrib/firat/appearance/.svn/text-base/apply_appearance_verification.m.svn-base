function det_list = apply_appearance_verification(detection_result_folder, det_list, dataset_path);
    addpath /home/firat/home.lems/Codes/nn/matlab_scripts
    if length(det_list) == 0
        return
    end
    output_dir='/home/firat/home.lems/Codes/nn/results';
    
    if 0 % create the dataset  
        tempfolder = ['/home/firat/home.lems/Codes/nn/dataset/' det_list(1).model_category];      
        mkdir(tempfolder);    
        for i = 1:length(det_list)
                det = det_list(i);
                imfile = [dataset_path '/' det.object_name '.jpg']; 
                xgraph_file = [detection_result_folder '/' det.xgraph_xml];
                I = imread(imfile);                
                s = size(I);
                ih = s(1);
                iw = s(2) ;
                %binary_mask = dbsksp_convert_xgraph_to_binary_mask(xgraph_file, iw, ih);
                %if isrgb(I)
                %    R = double(I(:,:,1)).* binary_mask;
                %    G = double(I(:,:,2)).* binary_mask;
                %    B = double(I(:,:,3)).* binary_mask;
                %    I(:,:,1) = R;  I(:,:,2) = G; I(:,:,3) = B;                   
                %else
                %    I = double(I).* binary_mask;
                %end
               
                %I = uint8(I);
                bb = round([det.bbox_xmin det.bbox_ymin det.bbox_xmax det.bbox_ymax]);
                bb(bb < 1) = 1;
                if bb(3) > iw
                    bb(3) = iw;
                end 
                if bb(4) > ih
                    bb(4) = ih;
                end
                w = bb(3) - bb(1) + 1;
                h = bb(4) - bb(2) + 1;
                IC = imcrop(I, [bb(1:2) w h]);
                out_imfile = [tempfolder '/' det_list(1).model_category '_' num2str(i) '.png'];
                imwrite(IC, out_imfile);
        end
        error('stop')
    end
    
    if 0
        %Create query set
        query_dir=[output_dir '/query'];
        mkdir(query_dir);
        grad_thresh=.1; %.07 used previously
        grid_spacing=3;
        %Create query set
        create_query_set(length(det_list),'/home/firat/home.lems/Codes/nn/dataset/',query_dir,grad_thresh,grid_spacing);
        curdir = pwd;
        cd /home/firat/home.lems/Codes/nn
        !ls results/query/*.pts > tempqueries.txt
        query_files = textread('tempqueries.txt', '%s');
        fid = [fopen('query1.txt', 'w'), fopen('query2.txt', 'w'), fopen('query3.txt', 'w'), fopen('query4.txt', 'w'), fopen('query5.txt', 'w'), fopen('query6.txt', 'w'), fopen('query7.txt', 'w'), fopen('query8.txt', 'w')];
        for i = 1:length(query_files)
             t = mod(i-1, length(fid)) + 1;
             fprintf(fid(t), '%s\n', query_files{i}); 
        end
        for i = 1:length(fid)
            fclose(fid(i)); 
        end
        fprintf('Please run the bash script!');
        return
    end
    %classification_dir = ['/home/firat/home.lems/Codes/nn/deprecated/results_old/model_nlabel_16/results'];
    classification_dir = '/home/firat/home.lems/Codes/nn/results/model_nlabel_1/results';
    to_be_removed = zeros(1, length(det_list));
    for i = 1:length(det_list)
        class_file = [classification_dir '/' det_list(1).model_category '_' num2str(i) '_' det_list(1).model_category '_results.txt'];
        [class_label, class_score] = textread(class_file, '%s %f');
        min_bg_score = Inf;
        for j = 1:length(class_label)
            if strcmp(class_label{j}, det_list(1).model_category)
                target_score = class_score(j);
            else
                if min_bg_score > class_score(j)
                    min_bg_score = class_score(j);
                end     
            end    
        end
        %score_ratio = class_score(bg_id) / class_score(cl_id);
        score_ratio = min_bg_score / target_score;
        if score_ratio <= 1.0087 %0.844 %0.865 %1 (ideal case)
           to_be_removed(i) = 1; 
        end
        
        %[min_score, min_score_i] = min(class_score);
        %if ~strcmp(det_list(1).model_category, class_label{min_score_i}) 
        %    to_be_removed(i) = 1;
        %end   
    end
    det_list(find(to_be_removed)) = [];
end
