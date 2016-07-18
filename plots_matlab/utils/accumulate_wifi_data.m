function res = accumulate_wifi_data (data, index)

for f=1:numel(index)
    res(f).nRun=0;
    res(f).samples=0;
    
    res(f).p=[];
    res(f).e=[];
    res(f).d=[];
    res(f).v=[];
    
    res(f).difTX=[];
    res(f).difRX=[];
    res(f).MaxBitrate=[];
    res(f).nPackets=[];
    res(f).pktSize=[];    
    res(f).totTime=[];  
    
    res(f).time=[];
end

%process data baseline
for i=1:numel(data)
    
    if numel(index)>1
        f = find(data{i}.freq == index);
    else
        f=1;
    end
    
    res(f).nRun=res(f).nRun+1;
    
    res(f).totTime=[res(f).totTime data{i}.endTime-data{i}.startTime];  
    res(f).difTX=[res(f).difTX data{i}.difTX];
    res(f).difRX=[res(f).difRX data{i}.difRX];
    res(f).MaxBitrate=[res(f).MaxBitrate data{i}.MaxBitrate];
    res(f).nPackets=[res(f).nPackets data{i}.nPackets];
    res(f).pktSize=[res(f).pktSize data{i}.pktSize];      
    
    res(f).samples = res(f).samples + numel(data{i}.power);
    
    res(f).p=[res(f).p; data{i}.power];
    res(f).e=[res(f).e; data{i}.energy];
    res(f).v=[res(f).v; data{i}.voltage];
    res(f).d = [res(f).d(:);data{i}.energy(1)-data{i}.energy(end)];
    
    res(f).time = [res(f).time data{i}.endTime - data{i}.startTime];
end

for f=1:numel(res)
    res(f).meanSamples = res(f).samples/res(f).nRun;
end
end