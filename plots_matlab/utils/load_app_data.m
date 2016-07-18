function [res]=load_app_data(input_dir,label,disp)

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

tline = fgets(fID); runID   = sscanf(tline,'RUN=%d');
tline = fgets(fID); name    = sscanf(tline,'name=%s');
tline = fgets(fID); width   = sscanf(tline,'width=%d');
tline = fgets(fID); height  = sscanf(tline,'height=%d');
tline = fgets(fID); fps     = sscanf(tline,'fps=%d');
tline = fgets(fID); realfps = sscanf(tline,'realfps=%f');

tline = fgets(fID); freq       = sscanf(tline,'freq=%f');
tline = fgets(fID); wifi       = sscanf(tline,'wifi=%s');
tline = fgets(fID); maxB       = sscanf(tline,'MaxBitrate=%f');

tline = fgets(fID); iniTX       = sscanf(tline,'iniTX=%s'); iniTX= str2double(iniTX(7:end));
tline = fgets(fID); endTX       = sscanf(tline,'endTX=%s'); endTX= str2double(endTX(7:end));
tline = fgets(fID); iniRX       = sscanf(tline,'iniRX=%s'); iniRX= str2double(iniRX(7:end));
tline = fgets(fID); endRX       = sscanf(tline,'endRX=%s'); endRX= str2double(endRX(7:end));

tline = fgets(fID); frames     = sscanf(tline,'frames=%d');

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

tline = fgets(fID); tline = tline(9:end-2);
tSenAct = str2double(strsplit (tline, ' '));
% ind = find(isnan(tSenAct) == 1); tSenAct(ind)=[];

tline = fgets(fID); tline = tline(9:end-2);
tProAct = str2double(strsplit (tline, ' '));
% ind = find(isnan(tProAct) == 1); tProAct(ind)=[];

tline = fgets(fID); tline = tline(9:end-2);
tComAct = str2double(strsplit (tline, ' '));
% ind = find(isnan(tProAct) == 1); tComAct(ind)=[];

tline = fgets(fID); tline = tline(9:end-2);
ComActTX = str2double(strsplit (tline, ' '));
% ind = find(isnan(tSenAct) == 1); ComActTX(ind)=[];

tline = fgets(fID); tline = tline(9:end-2);
ComActRX = str2double(strsplit (tline, ' '));
% ind = find(isnan(tSenAct) == 1); ComActRX(ind)=[];

res = struct ('file',filename,'runID',runID,'name',name,'frames',frames,...
    'width',width,'height',height,'freq',freq,'fps',fps,'realfps',realfps,...    
    'wifi',wifi,'maxBitrate',maxB,'iniTX',iniTX,'iniRX',iniRX,'endTX',endTX,'endRX',endRX,...
    'reads',reads,'time',time,'samples',samples,'startTime',startTime,'endTime',endTime,...
    'energy',energy,'power',power,'voltage',voltage,...
    'tSenAct',tSenAct,'tProAct',tProAct,'tComAct',tComAct,'ComActTX',ComActTX,'ComActRX',ComActRX);

fclose(fID);
end