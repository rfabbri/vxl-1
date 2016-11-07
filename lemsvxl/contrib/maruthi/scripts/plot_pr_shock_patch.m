function [] = plot_pr_shock_patch(file)

%Read in xml file
tree=xml_read(file);

%Grab positive and negative
numbPositive=tree.dataset.ATTRIBUTE.num_positive;
numbNegative=tree.dataset.ATTRIBUTE.num_negative;


%Numb pr points
prPoints = length(tree.dataset.stats);
precision = zeros(1,prPoints);
recall    = zeros(1,prPoints);

for i=1:prPoints
    
    %Grab stats
    stat = tree.dataset.stats(i);
    TP   = stat.TP;
    FP   = stat.FP;
    TN   = stat.TN;
    FN   = stat.FN;

    %Compute Precision
    precision(i)=TP/(TP+FP);
    
    %Compute Recall
    recall(i)=TP/numbPositive;
    
end
precision
recall
plot(recall,precision,'g*-','LineWidth',3);
axis([0 1 0 1]);
xlabel('recall','FontSize',12)
ylabel('precision','FontSize',12)
title(['PR curve for ' tree.dataset.ATTRIBUTE.name],'FontSize',12)