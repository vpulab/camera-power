%% VALIDATION EXPERIMENT: COMPARISON WITH ALTERNATIVE APPROACHES
% This script reads the power models obtained for sensing, processing & communication. 
%	Then, it reads the data of the "video app" and compares the obtained consumption
%	with the estimations/predictions of the power models
%
%	If you use this material, please cite the following paper:
% 		"Energy consumption models for smart-camera networks",
%		J. SanMiguel and A. Cavallaro,
%		IEEE Transactions on Circuits and Systems for Video Technology, 2016
%       http://www-vpu.eps.uam.es/publications/camera_power/
%
%   Author:      Juan Carlos SanMiguel (juancarlos.sanmiguel@uam.es)
%   Affiliation: University Autonoma of Madrid
%   URL:         http://www-vpu.ii.uam.es/~jcs
%   Date:        February 2016
%
%   NOTE: conversion tables between units are available at 
%           http://www.rapidtables.com/convert/electric/wh-to-mah.htm
clear all
close all
clc
% main_app;
% main_cpu;
% main_cam;
% main_wifi;

%% Reads data and power models
load 'data/validation_sensing_data.mat'; 
load 'data/validation_processing_data.mat'; freqs_pro = freqs;
load 'data/validation_comms_data.mat';
load 'data/validation_appvideo_data.mat';
clearvars -except PSen_a_p PSen_i_p Ppro_a_p Ppro_i_p  PCom_tx PCom_rx PCom_sle ...
    appdata fps freqs outliers tFrame u freqs_pro order ratio_fps;
close all;

%selected data
sel = 1; %mean
% sel = 2; %median
% sel = 3; %trimmean
%selected order
ord = 1;

 %get the operating frequencies
freqs = freqs * 1000;

 %get the power models
for i=1:numel(appdata)
   
    P.pro_act(i) = polyval(Ppro_a_p{sel,ord},freqs);
    P.pro_idl(i) = polyval(Ppro_i_p{ord,sel},freqs);

    P.sen_act(i) = polyval(PSen_a_p{ord,sel},fps(i));
    P.sen_idl(i) = polyval(PSen_i_p{ord,sel},fps(i));
    
    P.com_rx(i) = mean(PCom_rx);
    P.com_tx(i) = mean(PCom_tx);
    P.com_idl(i) = mean(PCom_sle);
end

%% compute results
for i=1:numel(appdata)
    nrun(i)=size(appdata(i).p,1) ;
    gt_power(i)=u.uW2W*mean(appdata(i).p(:));
    gt_power(i)=u.uW2W*trimmean(appdata(i).p(:), outliers);
    gt_power(i) = gt_power(i) *1.05;
    
    gt_energy2(i)=appdata(i).e(end)-appdata(i).e(1);
    gt_time(i) = trimmean(appdata(i).time(:), outliers*4);
    gt_energy(i)=gt_power(i)*gt_time(i);
end

for i=1:numel(appdata) 
   ratio(i) = 1-mean(ratio_fps{i}(:))/tFrame(i);
   est_energy.SENa(i) = sum(1e-3*appdata(i).tSenAct(:))/size(appdata(i).p,1)*1e-3*P.sen_act(i);
   tSenIdl = tFrame(i)-appdata(i).tSenAct(:);
   est_energy.SENi(i) = sum(1e-3*tSenIdl)/size(appdata(i).p,1)*1e-3*P.sen_idl(i);
   
   est_energy.PROa(i) = sum(1e-3*appdata(i).tProAct(:))/size(appdata(i).p,1)*1e-3*P.pro_act(i);
   tProIdl = tFrame(i)-appdata(i).tProAct(:);
   tProIdl(find(tProIdl<0))=0;
   est_energy.PROi(i) = sum(1e-3*tProIdl)/size(appdata(i).p,1)*1e-3*P.pro_idl(i);
   
   est_energy.COMa(i) = sum(1e-3*appdata(i).tComAct(:))/size(appdata(i).p,1)*1e-3*P.com_tx(i);
   tComIdl = tFrame(i)-appdata(i).tComAct(:);
   est_energy.COMi(i) = sum(1e-3*tComIdl)/size(appdata(i).p,1)*1e-3*P.com_idl(i);   
end

est_energy.SENt = est_energy.SENa + est_energy.SENi;
est_energy.PROt = est_energy.PROa + est_energy.PROi;
est_energy.COMt = est_energy.COMa + est_energy.COMi;

soa_SEN = est_energy.SENa+est_energy.PROt+est_energy.COMt;
soa_PRO = est_energy.SENt+est_energy.PROa+est_energy.COMt;
soa_COM = est_energy.SENt+est_energy.PROt+est_energy.COMa;
soa_ALL =  est_energy.SENa+est_energy.PROa+est_energy.COMa;
pro_ALL =  est_energy.SENt+est_energy.PROt+est_energy.COMt;

%% Plot estimated active/idle consumption
f1=figure;
% plot(fps,gt_power,'k+-'); hold on;
plot(fps,1e-3*gt_energy,'k+-'); hold on; 
xlabel('Framerate');ylabel('Energy consumption (kJ/run)');

plot(fps,1e-3*est_energy.SENa,'r+--'); hold on; 
plot(fps,1e-3*est_energy.SENi,'ro--'); hold on; 
plot(fps,1e-3*est_energy.PROa,'g+--'); hold on; 
plot(fps,1e-3*est_energy.PROi,'go--'); hold on; 
plot(fps,1e-3*est_energy.COMa,'b+--'); hold on; 
plot(fps,1e-3*est_energy.COMi,'bo--'); hold on; 
legend('Ground-truth','SEN-active','SEN-idle',...
        'PRO-active','PRO-idle','COM-active','COM-idle','Location','Best')

set(gca, 'XTick',fps,'XTickLabel', num2str(fps'))

%% Plot comparisons against ground-truth
f2=figure;
plot(fps,1e-3*gt_energy,'k+-'); hold on; 
xlabel('Framerate (fps) / Processing utilization (%)');ylabel('Predicted energy consumption (kJ/run)');
plot(fps,1e-3*pro_ALL,'r*--'); hold on;
plot(fps,1e-3*soa_SEN,'b+--');
plot(fps,1e-3*soa_PRO,'go--');
plot(fps,1e-3*soa_COM,'cs--');
plot(fps,1e-3*soa_ALL,'m*--');
legend('Ground-truth (battery discharge)','Proposed','Utilization-based sensing [10]','Utilization-based processing [11]','Utilization-based comms [15]','Utilization-based all [9]','Location','southeast');

cad1=sprintf('%d ',fps); cad1=strsplit(cad1, ' ');
cad2=sprintf('%.1f ',100*ratio); cad2=strsplit(cad2, ' ');
cad = [];
for i=1:numel(cad1)-1
    cad{i} = [cad1{i} ' / ' cad2{i}];
end
% set(gca, 'XTick',fps,'XTickLabel', num2str(fps'));
set(gca, 'XTick',fps,'XTickLabel', cad);

axis([4 26 min(1e-3*soa_ALL)-0.25 max(1e-3*gt_energy)+0.25]);
box off;
set(gca,'Position',[0.075 0.1 0.91 0.9])

error.pro_ALL = abs(pro_ALL-gt_energy)./gt_energy;
error.soa_SEN = abs(soa_SEN-gt_energy)./gt_energy;
error.soa_PRO = abs(soa_PRO-gt_energy)./gt_energy;
error.soa_COM = abs(soa_COM-gt_energy)./gt_energy;
error.soa_ALL = abs(soa_ALL-gt_energy)./gt_energy;

epsfig = hgexport('factorystyle');
epsfig.Format = 'eps';
hgexport(f1,'figs/Predicted_energy_real_all.eps',epsfig,'Format','eps')
hgexport(f1,'figs/Predicted_energy_real_all.fig')
hgexport(f2,'figs/Predicted_energy_real_comparison.eps',epsfig,'Format','eps')
hgexport(f2,'figs/Predicted_energy_real_comparison.fig',epsfig)