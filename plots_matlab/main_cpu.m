%% VALIDATION EXPERIMENT: CPU POWER MODELING
% This script computes the power model according to the readings obtained
% in the validation experiment for CPU modeling
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
outliers = 5; %in percentage
color={'b+-','m+-','go-','c-'};

%input data
input_dir = 'data/cpu/cpusel/';
dataset={'.'};
i=1;

%% read input data
data.act=load_cpu_data([input_dir dataset{i} '/'],'cpuact',true); %active
data.idl=load_cpu_data([input_dir dataset{i} '/'],'cpuidle');%idle
[freqs] = process_cpu_data(data.act); %test frequencies

%% process data - camera
act  = accumulate_cpu_data(data.act,freqs);
idl  = accumulate_cpu_data(data.idl,freqs);

% tests = accumulate(data.tests,sizes,fps);
volt = u.uV2V*mean(act(1).v(:)); %Voltage in volts

%% 1 - plot consumption data (active CPU, all runs)
nrows=numel(freqs);
ncols=2;
f1a = figure; 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(freqs)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(u.uW2mW*act(f).p',1:size(act(f).p,1),sprintf('Run (%d samples/run)',act(f).meanSamples),'Power (mW)',...
           sprintf('Active-%dMHz',act(f).freq));
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
        subplot(nrows,ncols,n*2+2);       
        plot_bar_data(u.A2mA * (u.uW2W * act(f).d)/volt, 1:numel(act(f).d), sprintf('Run (%d samples/run)',act(f).meanSamples),'Discharge (mAh)',...
           sprintf('Active-%dMHz',act(f).freq))
%        set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
       n=n+1;
end


%% 1 - plot consumption data (idle CPU, all runs)
nrows=numel(freqs);
ncols=2;
f1b = figure; 
set(gcf,'units','normalized','outerposition',[0 0 1 1])
n=0;
for f=1:numel(freqs)   
        subplot(nrows,ncols,n*2+1);           
        plot_stat_data(u.uW2mW*idl(f).p',1:size(idl(f).p,1),sprintf('Run (%d samples/run)',idl(f).meanSamples),'Power (mW)',...
           sprintf('Idle-%dMHz',idl(f).freq));
%       set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
      
      subplot(nrows,ncols,n*2+2);       
        plot_bar_data(u.A2mA * (u.uW2W * idl(f).d)/volt, 1:numel(idl(f).d), sprintf('Run (%d samples/run)',idl(f).meanSamples),'Discharge (mAh)',...
           sprintf('Idle-%dMHz',idl(f).freq))
%       set(gca, 'YTickLabel', num2str(get(gca, 'YTick')')) 
        n=n+1;
end

%% 2 - global data - active power
f2=figure('Name',[dataset{i} ' consumption active: overall' ], 'Position',[100 100 750 400]);
n=1;

    for f=1:numel(freqs)   
        l=bplot(u.uW2mW*act(f).p(:)',n,'nooutliers'); hold on;
%         l=bplot(u.uW2mW*act(f).p(:)',n,'outliers'); hold on;
        n=n+1;
    end

% set(gca,'Position',[0.075 0.1 0.925 0.85],'Xtick',1:numel(freqs), 'XtickLabel',[cellstr(num2str(freqs(1:end)', '%d'))']);
set(gca,'Xtick',1:numel(freqs), 'XtickLabel',[cellstr(num2str(freqs(1:end)', '%d'))']);
title(sprintf('Active consumption for CPU0, %d runs, %d samples/run, %.2f secs/sample',act(1).nRun,act(1).meanSamples,data.act{1}.time))
xlabel('CPU frequency (MHz)'); ylabel('Power (mW)'); legend(l(1:end-1),'Location','best');
% set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

%% 3 - global data - idle power
f3=figure('Name',[dataset{i} ' consumption idle: overall' ], 'Position',[100 100 750 400]);
n=1;

    for f=1:numel(freqs)   
        l=bplot(u.uW2mW*idl(f).p(:)',n,'nooutliers'); hold on;
%         l=bplot(u.uW2mW*act(f).p(:)',n,'outliers'); hold on;
        n=n+1;
    end

% set(gca,'Position',[0.075 0.1 0.925 0.85],'Xtick',1:numel(freqs), 'XtickLabel',[cellstr(num2str(freqs(1:end)', '%d'))']);
set(gca,'Xtick',1:numel(freqs), 'XtickLabel',[cellstr(num2str(freqs(1:end)', '%d'))']);
title(sprintf('Idle consumption for CPU0, %d runs, %d samples/run, %.2f secs/sample',act(1).nRun,act(1).meanSamples,data.act{1}.time))
xlabel('CPU frequency (MHz)'); ylabel('Power (mW)'); legend(l,'Location','eastoutside');
% set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

%% 4 comparison between active-idle power
f4=figure('Name',[dataset{i} ' consumption active-idle: overall' ]);
n=1;

    for f=1:numel(freqs)   
        l=bplot(u.uW2mW*act(f).p(:)',n,'nooutliers','whisker',outliers); hold on;
        l=bplot2(u.uW2mW*idl(f).p(:)',n,'nooutliers','whisker',outliers); hold on;
%         l=bplot(u.uW2mW*act(f).p(:)',n,'outliers'); hold on;
        n=n+1;
    end

set(gca,'Position',[0.125 0.1 0.87 0.88],'Xtick',1:numel(freqs), 'XtickLabel',[cellstr(num2str(freqs(1:end)', '%d'))']);
set(gca,'Xtick',1:numel(freqs), 'XtickLabel',[cellstr(num2str(freqs(1:end)', '%d'))']);
% title(sprintf('Power consumption for CPU0\n %d runs, %d samples/run, %.2f secs/sample',act(1).nRun,act(1).meanSamples,data.act{1}.time))
xlabel('CPU frequency (MHz)'); ylabel('Power consumption CPU0 (mW)'); legend(l(1:end-1),'Location','best');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
legend('Active-median','Active-mean','Active- 25%-75%','Active- 5%-95%','Idle-median','Idle-mean','Idle- 25%-75%','Idle- 5%-95%','Location','best')

%% 5 data for adjustment
%frequency-Voltage measurements 
for f=1:numel(freqs)
    Pact(1,f) = mean(act(f).p(:)*u.uW2mW);
    Pact(2,f) = median(act(f).p(:)*u.uW2mW);
    Pact(3,f) = trimmean(act(f).p(:)*u.uW2mW,outliers);
    
    Pidl(1,f) = mean(idl(f).p(:)*u.uW2mW);
    Pidl(2,f) = median(idl(f).p(:)*u.uW2mW);
    Pidl(3,f) = trimmean(idl(f).p(:)*u.uW2mW,outliers);
end
f5=figure; 
plot(freqs,Pact(1,:),'r+-',freqs,Pact(2,:),'bx-',freqs,Pact(3,:),'gs-'); hold on;
plot(freqs,Pidl(1,:),'r+--',freqs,Pidl(2,:),'bx--',freqs,Pidl(3,:),'gs--');
legend('Active-Mean','Active-Median','Active-Mean w/o outliers',...
       'Idle-Mean','Idle-Median','Idle-Mean w/o outliers','Location','best');
xlabel('CPU frequency (MHz)'); ylabel('Power (mW)');
title('CPU power consumption - Data for model fitting ');
% set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

%% fig 6 model fitting
%order of polynomial model fitting
order=[1 2 3];

sel=1;
ws = warning('off','all');  % Turn off warning
for i=1:numel(order)
    for d=1:3
        [Ppro_a_p{i,d},Ppro_a_err{i,d}] = polyfit(freqs,Pact(d,:),order(i));
        [Ppro_i_p{i,d},Ppro_i_err{i,d}] = polyfit(freqs,Pidl(d,:),order(i));        
    end
end
warning (ws);  % Turn on warning

% display results
f6=figure;
set(gcf,'units','normalized','outerposition',[0 0 1 1])
label={'Mean','Median','Mean w/o outliers'};
for d=1:3
    subplot (3,2,(d-1)*2+1)    
    plot(freqs,Pact(d,:),'kx','Markersize',6,'Linewidth',2); hold on;set(0, 'defaultTextInterpreter', 'tex');
    cad =[' legend (''Measurements ('  label{d} ') '''];
    for i=1:numel(order)
        pop_fit = polyval(Ppro_a_p{i,d},freqs,Ppro_a_err{i,d}); plot(freqs,pop_fit,color{i});  hold on;  
        cad = [cad [',''' sprintf('%d',i) '-order AvgErr=' sprintf('%.2f mW',Ppro_a_err{i,d}.normr/numel(freqs)) '''']];
    end
    eval ([cad ',''Location'', ''best'');']);
    xlabel('CPU frequency (MHz)'); ylabel('Power (mW)');
    title(sprintf('Active power fitting - %s',label{d}))    
    axis([freqs(1)-100 freqs(end)+100 min(Pact(:))-100 max(Pact(:))+100])
%     set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    
    subplot (3,2,(d-1)*2+2)
    plot(freqs,Pidl(d,:),'kx','Markersize',6,'Linewidth',2); hold on;set(0, 'defaultTextInterpreter', 'tex');
    cad =[' legend (''Measurements ('  label{d} ') '''];
    for i=1:numel(order)
        pop_fit = polyval(Ppro_i_p{i,d},freqs,Ppro_i_err{i,d}); plot(freqs,pop_fit,color{i});  hold on;  
        cad = [cad [',''' sprintf('%d',i) '-order AvgErr=' sprintf('%.2f mW',Ppro_i_err{i,d}.normr/numel(freqs)) '''']];
    end
    eval ([cad ',''Location'', ''best'');']);
    xlabel('CPU frequency (MHz)'); ylabel('Power (mW)');
    title(sprintf('Idle power fitting - %s',label{d}))
%     set(gca, 'YTickLabel', num2str(get(gca, 'YTick'))')
    axis([freqs(1)-100 freqs(end)+100 min(Pidl(:))-100 max(Pidl(:))+100])
%     set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
end

%% save figures
epsfig = hgexport('factorystyle');
epsfig.Format = 'eps';
hgexport(f1a,'figs/PRO_1_runs_active.eps',epsfig,'Format','eps')
hgexport(f1b,'figs/PRO_1_runs_idle.eps',epsfig,'Format','eps')
hgexport(f2,'figs/PRO_2_overall_active.eps',epsfig,'Format','eps')
hgexport(f3,'figs/PRO_2_overall_idle.eps',epsfig,'Format','eps')
hgexport(f4,'figs/PRO_3_active-vs-idle.eps',epsfig,'Format','eps')  
hgexport(f5,'figs/PRO_4_fitting_data2model.eps',epsfig,'Format','eps')  
hgexport(f6,'figs/PRO_4_fitting_results.eps',epsfig,'Format','eps')  

%save file with all variables for further processing
save('data/validation_processing_data.mat'); 