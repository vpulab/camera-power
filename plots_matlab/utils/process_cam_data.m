function [sizes, fps]=process_cam_data(data)

sizes = [];
fps = [];
sizes=[data{1}.width data{1}.height];
for i=1:numel(data)
    
    for s=1:size(sizes,1)
        if (sizes(s,1) ~= data{i}.width || sizes(s,2) ~= data{i}.height)
            if (data{i}.width>0 && data{i}.height>0)
                sizes=[sizes; data{i}.width data{i}.height]
            end
        end
    end
    
    fps=[fps data{i}.fps];    
end

fps=unique(fps,'sorted');
