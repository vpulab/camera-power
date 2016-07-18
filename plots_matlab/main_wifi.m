%% VALIDATION EXPERIMENT: WIFI POWER MODELING
% This script computes the power model according to the readings obtained
% in the validation experiment for WIFI modeling
%
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

addpath(genpath('utils/'));
%% configuration
u.uW2W=1e-6; %uW to W
u.uW2mW=1e-3; %uW to mW
u.uV2V=1e-6; %uVolts to Volts
u.A2mA=1e3;%Amperes to miliAmperes
outliers = 15; %in percentage
color={'b+-','m+-','go-','c-'};

%input data
input_dir = 'data/wifi/';

%% read input data
data.base=load_wifi_data(input_dir,'system',true); %active
data.stdby=load_wifi_data(input_dir,'system_usb');%idle
data.rx=load_wifi_data(input_dir,'wifiRX');%idle
data.tx=load_wifi_data(input_dir,'wifiTX');%idle

%% process data - camera
base  = accumulate_wifi_data(data.base,1);
stdby  = accumulate_wifi_data(data.stdby,1);
rx  = accumulate_wifi_data(data.rx,1);
tx  = accumulate_wifi_data(data.tx,1);

KBps  = process_wifi_data(data.rx);

% tests = accumulate(data.tests,sizes,fps);
volt = u.uV2V*mean(base.v(:)); %Voltage in volts

%% 1 - runs for consumption data
f1 = figure('Name','Power consumption: runs');
set(gcf,'units','normalized','outerposition',[0 0 1 1])

subplot 421
plot_stat_data(u.uW2mW*base.p',1:size(base.p,1), sprintf('Run (%d samples/run)',base.meanSamples),'Power (mW)','Baseline (usb OFF)');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
subplot 422
plot_bar_data(u.A2mA * (u.uW2W * base.d) / volt, 1:numel(base.d),sprintf('Run (%d samples/run)',base.meanSamples),'Discharge (mAh)','Baseline (usb OFF)')
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

subplot 423
plot_stat_data(u.uW2mW*stdby.p',1:size(stdby.p,1), sprintf('Run (%d samples/run)',stdby.meanSamples),'Power (mW)','Standby (usb ON)');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
subplot 424
plot_bar_data(u.A2mA * (u.uW2W * stdby.d) / volt, 1:numel(stdby.d), sprintf('Run (%d samples/run)',stdby.meanSamples),'Discharge (mAh)','Standby (usb ON)')
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

subplot 425
plot_stat_data(u.uW2mW*rx.p',1:size(rx.p,1), sprintf('Run (%d samples/run)',rx.meanSamples),'Power (mW)',' RX mode');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
subplot 426
plot_bar_data(u.A2mA * (u.uW2W * rx.d) / volt, 1:numel(rx.d), sprintf('Run (%d samples/run)',rx.meanSamples),'Discharge (mAh)','RX mode')
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

subplot 427
plot_stat_data(u.uW2mW*tx.p',1:size(tx.p,1), sprintf('Run (%d samples/run)',tx.meanSamples),'Power (mW)',' TX mode');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
subplot 428
plot_bar_data(u.A2mA * (u.uW2W * tx.d) / volt, 1:numel(tx.d), sprintf('Run (%d samples/run)',tx.meanSamples),'Discharge (mAh)','TX mode')
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

%% 2 - runs for throughput data
f2 = figure('Name','Data exchanged: runs','Position',[100 100 1050 500]);

for r=1:numel(KBps)
    subplot 121;
    plot(1:numel(rx(r).difRX),1e-3*rx(r).difRX./rx(r).totTime,'r+-'); hold on;
    plot(1:numel(rx(r).difTX),1e-3*rx(r).difTX./rx(r).totTime,'bx-');
    axis([0.5 numel(rx(r).difTX)+0.5 min(1e-3*rx(r).difTX./rx(r).totTime)-100 max(1e-3*rx(r).difRX./rx(r).totTime)+100])
    title('RX mode'); xlabel(sprintf('Run (%d samples/run)',rx(r).meanSamples)); ylabel('Kbits/sec')
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    set(gca, 'XTick', 1:numel(rx(r).difTX))
    legend('RX throughput','TX throughput','Location','best');

    subplot 122;
    plot(1:numel(tx(r).difRX),1e-3*tx(r).difRX./tx(r).totTime,'r+-'); hold on;
    plot(1:numel(tx(r).difTX),1e-3*tx(r).difTX./tx(r).totTime,'bx-');
    axis([0.5 numel(tx(r).difTX)+0.5 min(1e-3*tx.difRX./tx(r).totTime)-100 max(1e-3*tx(r).difTX./tx(r).totTime)+100])
    title('TX mode'); xlabel(sprintf('Run (%d samples/run)',rx(r).meanSamples)); ylabel('Kbits/sec')
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    set(gca, 'XTick', 1:numel(rx(r).difTX))
    legend('RX throughput','TX throughput','Location','best');

    annotation(f2,'textbox',[0.35 0.95 0.65 0.05],'String',{'Communication performance (limited to 1000Kbps)'},'FitBoxToText','on');
end
%% 3 data for adjustment

PCom_sle(1) = mean(stdby.p(:)*u.uW2mW);    
offset = trimmean(base.p(:),outliers);
for r=1:numel(KBps)    
    PCom_sle(1,r) = u.uW2mW*(mean(stdby.p(:))-offset);    
    PCom_sle(2,r) = u.uW2mW*(median(stdby.p(:))-offset);    
    PCom_sle(3,r) = u.uW2mW*(trimmean(stdby.p(:),outliers)-offset);    

    PCom_rx(1,r) = u.uW2mW*(mean(rx(r).p(:))-offset);    
    PCom_rx(2,r) = u.uW2mW*(median(rx(r).p(:))-offset);
    PCom_rx(3,r) = u.uW2mW*(trimmean(rx(r).p(:),outliers)-offset);
    
    PCom_tx(1,r) = u.uW2mW*(mean(tx(r).p(:))-offset);
    PCom_tx(2,r) = u.uW2mW*(median(tx(r).p(:))-offset);
    PCom_tx(3,r) = u.uW2mW*(trimmean(tx(r).p(:),outliers)-offset);
end
% f3=figure('Position',[100 100 1500 400]); 
% plot(KBps,Prx(1,:),'r+-',KBps,Prx(2,:),'bx-',KBps,Prx(3,:),'gs-'); hold on;
% plot(KBps,Ptx(1,:),'r+--',KBps,Ptx(2,:),'bx--',KBps,Ptx(3,:),'gs--');
% legend('Active-Mean','Active-Median','Active-Mean w/o outliers',...
%        'Idle-Mean','Idle-Median','Idle-Mean w/o outliers','Location','northwest');
% xlabel('Bitrate (Kbytes/sec)'); ylabel('Power (mW)');
save
f3=figure;
bar([PCom_sle'; PCom_rx'; PCom_tx']);
legend('Mean','Median','Mean w/o outliers','Location','eastoutside');
xlabel('Communication status'); ylabel('Power (mW)');
title('Results for model fitting (wrt baseline power) ');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
set(gca, 'XTickLabel', {'Standby','RX mode','TX mode'})


%% power: baseline vs standby vs active vs idle
f10 = figure('Name',['consumption: baseline vs standby vs active vs idle' ]);
% bplot(u.uW2mW*base.p(:)',1,'nooutliers'); hold on;
Psleep(1) = mean(base.p(:)); 
bplot(u.uW2mW*(stdby.p(:)-Psleep)',1,'nooutliers'); hold on;
bplot(u.uW2mW*(rx(1).p(:)-Psleep)',2,'nooutliers'); hold on;
l=bplot(u.uW2mW*(tx(1).p(:)-Psleep)',3,'nooutliers'); hold on;

% axis([0.5 numel(fps)+2.5 min(u.uW2mW*base.p(:))-100 1.22e4 ])
% xlabel(sprintf('Run (%d samples/run)',base.meanSamples))
ylabel('Power consumption (mW)');
set(gca, 'Xtick', 1:4,'XTickLabel', {'Idle','RX mode','TX mode'})
xlabel('Communication');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
%title('Consumption for USB camera (640x480) \newline Active (top)-vs-Idle (bottom) ');
legend(l(1:end-1),'location','best')
set(gca,'Position',[0.1 0.1 0.9 0.87]);

%% save figures
epsfig = hgexport('factorystyle');
epsfig.Format = 'eps';
hgexport(f1,'figs/COM_1_runs_active.eps',epsfig,'Format','eps')
hgexport(f2,'figs/COM_2_bitrate.eps',epsfig,'Format','eps')
hgexport(f3,'figs/COM_3_fitting_results.eps',epsfig,'Format','eps')

save('data/validation_comms_data.mat');