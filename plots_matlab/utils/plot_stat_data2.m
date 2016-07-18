function plot_stat_data2(data, pos, xlabel_str,ylabel_str,title_str)

l=bplot(data,pos,'nooutliers');
% legend(l(1:end-1),'Location','eastoutside');
% set(gca,'Xtick',1:numel(legend_str), 'XtickLabel',legend_str);
xlabel(xlabel_str);
ylabel(ylabel_str);
title(title_str);
