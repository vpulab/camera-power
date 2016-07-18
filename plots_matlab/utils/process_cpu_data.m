function [freq]=process_cpu_data(data)

freq = [];
for i=1:numel(data)    
    freq=[freq data{i}.freq];    
end

freq=unique(freq,'sorted');
