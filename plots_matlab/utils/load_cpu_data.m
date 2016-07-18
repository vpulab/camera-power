function [res]=load_cpu_data(input_dir,label,disp)

if nargin < 3
    disp=false;
end
if isempty(label)
    label = -1;
end

files = dir([input_dir '*.dat']);
h = waitbar(0,'Loading data');
for f=1:numel(files)
    if (disp)
        fprintf('Processing file %s...\n',char(files(f).name));
    end
    res{f} = parse_file([input_dir char(files(f).name)]);    
    waitbar(f/numel(files));
end

close(h);

ind=[];
for f=1:numel(files)
    if (label ~=-1)
        if ~strcmp(label,res{f}.name)
            ind = [ind f];
        end
    end
end
if~isempty(ind)
    res(ind)=[];
end

maxSamples=0;
minSamples=Inf;
for f=1:numel(res)
%     [f size(res{f}.energy,2)]
    maxSamples = max(maxSamples, size(res{f}.energy,2));
    minSamples = min(minSamples, size(res{f}.energy,2));
end
    
for f=1:numel(res)    
    res{f}.energy = res{f}.energy(1:minSamples);
    res{f}.power = res{f}.power(1:minSamples);
    res{f}.voltage = res{f}.voltage(1:minSamples);   
end


end

function [res]=parse_file(filename)

fID = fopen(filename,'r');

tline = fgets(fID); runID = sscanf(tline,'RUN=%d');
tline = fgets(fID); name  = sscanf(tline,'name=%s');
% tline = fgets(fID); width = sscanf(tline,'width=%d');
% tline = fgets(fID); height= sscanf(tline,'height=%d');

tline = fgets(fID); freq       = sscanf(tline,'freq=%d');
tline = fgets(fID); reads     = sscanf(tline,'reads=%d');
tline = fgets(fID); time      = sscanf(tline,'time=%f');
tline = fgets(fID); samples  = sscanf(tline,'samples=%f');
tline = fgets(fID); startTime = sscanf(tline,'start_time=%f');
tline = fgets(fID); endTime   = sscanf(tline,'end_time=%f');

tline = fgets(fID); tline = tline(8:end-2);
energy = str2double(strsplit (tline, ' '));
% ind = find(isnan(energy) == 1); energy(ind)=[];

tline = fgets(fID); tline = tline(7:end-2);
power = str2double(strsplit (tline, ' '));
% ind = find(isnan(power) == 1); power(ind)=[];

tline = fgets(fID); tline = tline(9:end-2);
voltage = str2double(strsplit (tline, ' '));
% ind = find(isnan(voltage) == 1); voltage(ind)=[];

% tline = fgets(fID); tline = tline(10:end-2);
% captureT = str2double(strsplit (tline, ' '));
% ind = find(isnan(captureT) == 1); captureT(ind)=[];

% tline = fgets(fID); tline = tline(8:end-2);
% sleepT = str2double(strsplit (tline, ' '));
% ind = find(isnan(sleepT) == 1); sleepT(ind)=[];

res = struct ('file',filename,'runID',runID,'name',name,'freq',freq,...    
    'reads',reads,'time',time,'samples',samples,'startTime',startTime,'endTime',endTime,...
    'energy',energy,'power',power,'voltage',voltage);

fclose(fID);
end