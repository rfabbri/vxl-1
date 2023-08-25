function [ best_pt, outside_init_range, outside_final_range, prev_sample, next_sample ] = find_closest_point_on_curve(rec,pt)

    num_samples = size(rec,1);
    best_dist = 10^6;
    best_pt = -1;
    prev_sample = -1;
    next_sample = -1;
    outside_init_range = -1;
    outside_final_range = -1;
    
    
    for s=2:num_samples
        
        ax = rec(s-1,1);
        ay = rec(s-1,2);
        az = rec(s-1,3);
        a = [ax ay az];
        
        bx = rec(s,1);
        by = rec(s,2);
        bz = rec(s,3);
        b = [bx by bz];
        
        qx = pt(1,1);
        qy = pt(1,2);
        qz = pt(1,3);
        q = [qx qy qz];
        
        tnom = (ax-bx)*(ax-qx) + (ay-by)*(ay-qy) + (az-bz)*(az-qz);
        tdenom = (ax-bx)^2 + (ay-by)^2 + (az-bz)^2;
        
        t = tnom/tdenom;
        clipped_init = 0;
        clipped_final = 0;
        if(t<0)
            t=0;
            clipped_init = 1;
        elseif(t>1)
            t=1;
            clipped_final = 1;
        end
        closest_pt = a + t*(b-a);
        closest_dist = norm(closest_pt-pt);
        
        if(closest_dist<best_dist)
            best_pt = closest_pt;
            best_dist = closest_dist;
            outside_init_range = clipped_init;
            outside_final_range = clipped_final;
            prev_sample = s-1;
            next_sample = s;
        end       
        
    end

end

