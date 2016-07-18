function res = accumulate_cpu_data (data, index)

for f=1:numel(index)
    res(f).nRun=0;
    res(f).samples=0;
    
    res(f).p=[];
    res(f).e=[];
    res(f).d=[];
    res(f).v=[];
    
    res(f).aTime=[];
    res(f).sTime=[];
    
    res(f).time = [];
end

%process data baseline
for i=1:numel(data)
    
    if numel(index)>1
        f = find(data{i}.freq == index);
    else
        f=1;
    end
    
    res(f).nRun=res(f).nRun+1;
    res(f).freq = data{i}.freq;
    
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
% %process data baseline
% result.p=[]; %power
% result.e=[];
% result.v=[];
% result.d=[];
% result.captureT=[];
% result.sleepT=[];
% result.numRuns=0;
% result.width=data{1}.width;
% result.height=data{1}.height;
% result.fps = data{1}.fps;
% result.samples=0;
%
% for i=1:numel(data)
%     result.p=[result.p;data{i}.p];
%     result.e=[result.e; data{i}.e];
%     result.discharge = [result.discharge; data{i}.e(1)-data{i}.e(end)];
%     result.v=[result.v; data{i}.v];
%     result.captureT=[result.captureT; data{i}.captureT];
%     result.sleepT=[result.sleepT;data{i}.sleepT];
%     result.numRuns=result.numRuns+1;
%     result.samples = result.samples + numel(data{i}.p);
% end
%
% result.meanSamples = result.samples/result.numRuns;
