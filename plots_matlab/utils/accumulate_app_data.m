function res = accumulate_app_data (data, index)

for f=1:numel(index)
    res(f).nRun=0;
    res(f).samples=0;
    
    res(f).p=[];
    res(f).e=[];
    res(f).d=[];
    res(f).v=[];

    res(f).tSenAct=[];
    res(f).tProAct=[];
    res(f).tComAct=[];
    
    res(f).ComActTX=[];
    res(f).ComActRX=[];
    res(f).ComRealTX = [];
    res(f).ComRealRX = [];
    res(f).time = [];
end

datlen = [2500 7500 9000];
%process data baseline
for i=1:numel(data)
    
    if numel(index)>1
        f = find(data{i}.fps == index);
    else
        f=1;
    end
    
    res(f).nRun=res(f).nRun+1;
    res(f).freq = data{i}.freq;
    res(f).fps = data{i}.fps;
    
    res(f).samples = res(f).samples + numel(data{i}.power);
    
    res(f).p=[res(f).p; data{i}.power];
    res(f).e=[res(f).e; data{i}.energy];
    res(f).v=[res(f).v; data{i}.voltage];
    res(f).d = [res(f).d(:);data{i}.energy(1)-data{i}.energy(end)];
    
    res(f).tSenAct=[res(f).tSenAct; data{i}.tSenAct(1:datlen(f))];
    res(f).tProAct=[res(f).tProAct; data{i}.tProAct(1:datlen(f))];
    res(f).tComAct=[res(f).tComAct; data{i}.tComAct(1:datlen(f))];
    res(f).ComActTX=[res(f).ComActTX; data{i}.ComActTX(1:datlen(f))];
    res(f).ComActRX=[res(f).ComActRX; data{i}.ComActRX(1:datlen(f))];
    
    res(f).ComRealTX = [res(f).ComRealTX; (data{i}.endTX-data{i}.iniTX)];
    res(f).ComRealRX = [res(f).ComRealRX; (data{i}.endRX-data{i}.iniRX)];
    
    res(f).time = [res(f).time data{i}.endTime - data{i}.startTime];
    
end

for f=1:numel(res)
    res(f).meanSamples = res(f).samples/res(f).nRun;
end
end
