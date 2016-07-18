function plot_stat_data(data, legend_str, xlabel_str,ylabel_str,title_str)

l=bplot(data,'nooutliers');
% legend(l(1:end-1),'Location','eastoutside');
set(gca,'Xtick',1:numel(legend_str), 'XtickLabel',legend_str);
xlabel(xlabel_str);
ylabel(ylabel_str);
title(title_str);
