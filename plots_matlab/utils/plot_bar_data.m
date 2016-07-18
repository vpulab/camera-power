function plot_bar_data(data, legend_str, xlabel_str,ylabel_str,title_str)

bar(data);
set(gca,'Xtick',1:numel(legend_str), 'XtickLabel',legend_str);
% axis([0 numel(legend_str)+1 0 max(abs(data(:))+eps) ]);
xlabel(xlabel_str);
ylabel(ylabel_str);
title(title_str);