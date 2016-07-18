function [freq, fps]=process_app_data(data)

freq = [];
fps = [];
for i=1:numel(data)    
    freq=[freq data{i}.freq];    
    fps=[fps data{i}.fps]; 
end

freq=unique(freq,'sorted');
fps=unique(fps,'sorted');
