%% test data reconstruction accurracy..

for i=0:9
    
    eval(sprintf('P=house_%gp',i));
    M=house_p3d;
    M(:,4)=1;
    
    eval(sprintf('corner_list = house%g_corn;',i));
    
    count=0;
    corner_proj = P*M';

    count=0;
    for j=1:672
        index  = house_corn(j,i+1);
        m_proj = corner_proj(:,j);
        x_proj = m_proj(1)/m_proj(3);
        y_proj = m_proj(2)/m_proj(3);
        
        if index~=0
            count=count+1;
            x_data = corner_list(index+1,1);
            y_data = corner_list(index+1,2);
            
            proj_err(i+1,count) = sqrt((x_proj-x_data)^2 + (y_proj-y_data)^2);
        end
    end
            
end
