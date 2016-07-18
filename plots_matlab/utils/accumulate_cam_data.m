function res = accumulate_cam_data(data, index1, index2)


if(nargin == 1)
    index1=[0,0];
    index2=[0];
end

for s=1:size(index1,1)
    for f=1:numel(index2)
        res(s,f).nRun=0;
        res(s,f).samples=0;        
        
        res(s,f).p=[];
        res(s,f).e=[];
        res(s,f).d=[];
        res(s,f).v=[];
        
        res(s,f).aTime=[];
        res(s,f).sTime=[];     
        
        res(f).time = [];
    end
end

%process data baseline
for i=1:numel(data)
    
    if size(index1(:,1),1)>1 || numel(index2)>1
        s = find(data{i}.width == index1(:,1));
        f = find(data{i}.fps == index2);
    else
        s=1;
        f=1;
    end

    res(s,f).nRun=res(s,f).nRun+1;
    res(s,f).width = data{i}.width;
    res(s,f).height = data{i}.height;
    res(s,f).fps = data{i}.fps;
    
    res(s,f).samples = res(s,f).samples + numel(data{i}.power);
    
    res(s,f).p=[res(s,f).p; data{i}.power];
    res(s,f).e=[res(s,f).e; data{i}.energy];
    res(s,f).v=[res(s,f).v; data{i}.voltage];
    res(s,f).d = [res(s,f).d(:);data{i}.energy(1)-data{i}.energy(end)];     
    
    res(s,f).aTime=[res(s,f).aTime; data{i}.captureT];
    res(s,f).sTime=[res(s,f).sTime; data{i}.sleepT];
    
    res(f).time = [res(f).time data{i}.endTime - data{i}.startTime];

end

for s=1:size(res,1)
    for f=1:size(res,2)
        res(s,f).meanSamples = res(s,f).samples/res(s,f).nRun;
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
