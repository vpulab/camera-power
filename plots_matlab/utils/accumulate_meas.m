function res = accumulate_meas(data, sizes,fps)

for s=1:size(sizes,1)
    for f=1:numel(fps)    
        res(s,f).p=[];
        res(s,f).e=[];
        res(s,f).captureT=[];
        res(s,f).sleepT=[];
        res(s,f).numRuns=0;
        res(s,f).disc=[];
        res(s,f).v=[];
        res(s,f).samples=0;
    end
end

for i=1:numel(data)    
    s = find(data{i}.width == sizes(:,1));
    f = find(data{i}.fps == fps);
    
    res(s,f).width = data{i}.width;
    res(s,f).height = data{i}.height;
    res(s,f).fps = data{i}.fps;
    res(s,f).p=[res(s,f).p; data{i}.p];
    res(s,f).e1=[res(s,f).e; data{i}.e];
    res(s,f).captureT=[res(s,f).captureT; data{i}.captureT];
    res(s,f).sleepT=[res(s,f).sleepT; data{i}.sleepT];
    res(s,f).v=[res(s,f).v; data{i}.v];
    res(s,f).numRuns=res(s,f).numRuns+1;
    res(s,f).samples = res(s,f).samples + numel(data{i}.p);
    res(s,f).disc = [res(s,f).disc(:);data{i}.e(1)-data{i}.e(end)];
end

for i=1:numel(data)    
    s = find(data{i}.width == sizes(:,1));
    f = find(data{i}.fps == fps);
    res(s,f).meanSamples = res(s,f).samples/res(s,f).numRuns;
end