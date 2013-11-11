function mybar3(Z, xticks, yticks, zticks, xlabel1, ylabel1, zlabel1)
        h = bar3(Z);
        axis([0.5 length(xticks)+0.5 0.5 length(yticks)+0.5 zticks(1) zticks(end)])
        shading interp
        for i = 1:length(h)
                zdata = get(h(i),'Zdata');
                set(h(i),'Cdata',zdata)
                set(h,'EdgeColor','k')
        end
        set(gca, 'XTick', 1:length(xticks))
        set(gca, 'XTickLabel', xticks)
        set(gca, 'YTick', 1:length(yticks))
        set(gca, 'YTickLabel', yticks)
        %set(gca, 'ZTick', 1:length(zticks))
        %set(gca, 'ZTickLabel', zticks)
        xlabel(xlabel1, 'FontSize', 20); ylabel(ylabel1, 'FontSize', 20); zlabel(zlabel1, 'FontSize', 20)
    
end


