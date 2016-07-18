%% VALIDATION EXPERIMENT: VIDEO APP POWER MODELING
% This script computes the power model according to the readings obtained
% in the validation experiment for VIDEO APP modeling
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

% http://www.rapidtables.com/convert/electric/wh-to-mah.htm
clear all;
close all;
clc;

addpath(genpath('utils/'));
%% configuration
u.uW2W=1e-6; %uW to W
u.uW2mW=1e-3; %uW to mW
u.uV2V=1e-6; %uVolts to Volts
u.A2mA=1e3;%Amperes to miliAmperes
outliers = 5; %in percentage
color={'b+-','m+-','go-','c-'};

%input data
input_dir = 'data/appvideo/';
dataset={'.'};
i=1;

%% read input data
data=load_app_data([input_dir dataset{i} '/'],'appvideo',true); %active
[freqs,fps] = process_app_data(data); %test frequencies

%% process data - video application
appdata = accumulate_app_data(data,fps);

% tests = accumulate(data.tests,sizes,fps);
volt = u.uV2V*mean(appdata(1).v(:)); %Voltage in volts
tFrame = 1e3*1./fps; %in msec

%% 1a - plot consumption data
nrows=numel(fps);
ncols=2;
f1a = figure('Name','Overall consumption data'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(u.uW2mW*appdata(f).p',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Power (mW)',...
           sprintf('Application - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
        subplot(nrows,ncols,n*2+2);       
        plot_bar_data(u.A2mA * (u.uW2W * appdata(f).d)/volt, 1:numel(appdata(f).d), sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Discharge (mAh)',...
           sprintf('Application - %d fps',appdata(f).fps))
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
       n=n+1;
end

%% 1b - plot active data
nrows=numel(fps);
ncols=3;
f1b = figure('Name','Overall activation time'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        subplot(nrows,ncols,n*3+1);           
        plot_stat_data(appdata(f).tSenAct',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Sensing - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
        subplot(nrows,ncols,n*3+2);       
        plot_stat_data(appdata(f).tProAct',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Processing - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

        subplot(nrows,ncols,n*3+3);       
        plot_stat_data(appdata(f).tComAct',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Comms - %d fps',appdata(f).fps));
       axis([ 0.5 size(appdata(f).p,1)+0.5 0.02 0.08 ]);
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
       n=n+1;
end

%% 1c - plot RX/TX data
nrows=numel(fps);
ncols=2;
f1c = figure('Name','Overall RX/TX time'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(appdata(f).ComActRX',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('RX data per frame - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
        subplot(nrows,ncols,n*2+2);       
        plot_stat_data(appdata(f).ComActTX',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('TX data per frame - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

       n=n+1;
end

%% 1d - plot RX/TX data
nrows=numel(fps);
ncols=2;
f1d = figure('Name','Overall RX/TX data'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(appdata(f).ComRealRX',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('RX data per frame - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
        subplot(nrows,ncols,n*2+2);       
        plot_stat_data(appdata(f).ComRealTX',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('TX data per frame - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

       n=n+1;
end


%% 2a - plot active data sensing
nrows=numel(fps);
ncols=2;
f2a = figure('Name','Active-idle time for sensing'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(appdata(f).tSenAct',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Active Sensing - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
        subplot(nrows,ncols,n*2+2);       
        plot_stat_data((tFrame(f)-appdata(f).tSenAct)',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Idle Processing - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
       n=n+1;
end

%% 2b - plot active data processing
nrows=numel(fps);
ncols=2;
f2b = figure('Name','Active-idle time for processing'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(appdata(f).tProAct',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Active processing - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
        subplot(nrows,ncols,n*2+2);       
        ratio = tFrame(f)-appdata(f).tProAct;
        ratio(find(ratio<0))=0;
        plot_stat_data(ratio',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Idle Processing - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
       n=n+1;
       ratio_fps{f}=ratio;
end

%% 2c - plot active data comms
nrows=numel(fps);
ncols=2;
f2c = figure('Name','Active-idle time for comms'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(appdata(f).tComAct',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Active comms - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
       axis([ 0.5 size(appdata(f).p,1)+0.5 0.02 0.08 ]);
       
        subplot(nrows,ncols,n*2+2);       
        plot_stat_data((tFrame(f)-appdata(f).tComAct)',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'Active time (ms)',...
           sprintf('Idle comms - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
        meanT = tFrame(f)-appdata(f).tComAct;
        meanT = trimmean(meanT(:), 25);
       axis([ 0.5 size(appdata(f).p,1)+0.5  meanT-0.02 meanT+0.02 ]);
       n=n+1;
end

%% 2d - plot active data comms
nrows=numel(fps);
ncols=1;
f2d = figure('Name','Active-idle time globally'); 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(fps)   
        tTot = appdata(f).tSenAct+appdata(f).tProAct+appdata(f).tComAct;
        tTot = tTot./tFrame(f);
        tTot(find(tTot>1))=1;
        
        subplot(nrows,ncols,n*1+1);           
        plot_stat_data(tTot',1:size(appdata(f).p,1),sprintf('Run (%d samples/run)',appdata(f).meanSamples),'CPU Load (%)',...
           sprintf('CPU load - %d fps',appdata(f).fps));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
     
       n=n+1;
end


% 
% %% save figures
% epsfig = hgexport('factorystyle');
% epsfig.Format = 'eps';
% hgexport(f1a,'figs/PRO_1_runs_active.eps',epsfig,'Format','eps')
% hgexport(f1b,'figs/PRO_1_runs_idle.eps',epsfig,'Format','eps')
% hgexport(f2,'figs/PRO_2_overall_active.eps',epsfig,'Format','eps')
% hgexport(f3,'figs/PRO_2_overall_idle.eps',epsfig,'Format','eps')
% hgexport(f4,'figs/PRO_3_active-vs-idle.eps',epsfig,'Format','eps')  
% hgexport(f5,'figs/PRO_4_fitting_data2model.eps',epsfig,'Format','eps')  
% hgexport(f6,'figs/PRO_4_fitting_results.eps',epsfig,'Format','eps')  

save('data/validation_appvideo_data.mat');