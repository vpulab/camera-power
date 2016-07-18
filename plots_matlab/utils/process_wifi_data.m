function [MaxKBps]=process_wifi_data(data)

MaxKBps = [];
for i=1:numel(data)    
    MaxKBps=[MaxKBps data{i}.MaxBitrate*1/8];    
end

MaxKBps=unique(MaxKBps,'sorted');
