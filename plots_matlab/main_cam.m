%% VALIDATION EXPERIMENT: SENSING POWER MODELING
% This script computes the power model according to the readings obtained
% in the validation experiment for sensing (USB camera) modeling
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
input_dir = 'data/usbcam/setall/';
dataset={'active','idle'};

for i=1:numel(dataset)
    
    %% read input data
    data.base=load_cam_data([input_dir 'idle/'],'system',true); %baseline- webcam usb off
    data.stdby=load_cam_data([input_dir 'idle/'],'system_usb');%baseline - webcam usb on
    data.tests{i}=load_cam_data([input_dir dataset{i} '/' ],'cam1');%tests{i}
    
    [sizes fps] = process_cam_data(data.tests{i});
    
    %% process data - camera
    base  = accumulate_cam_data(data.base);
    stdby = accumulate_cam_data(data.stdby);
    tests{i} = accumulate_cam_data(data.tests{i},sizes,fps);
    volt = u.uV2V*mean(base.v(:)); %Voltage in volts
    
    %% 1 - plot consumption data
    nrows=numel(fps)*size(sizes,1)+2;
    ncols=2;
    
    f1 = figure('Name',[dataset{i} ' consumption: runs' ]);
    set(gcf,'units','normalized','outerposition',[0 0 1 1])
    
    subplot(nrows,ncols,1)
    plot_stat_data(u.uW2mW*base.p',1:size(base.p,1), sprintf('Run (%d samples/run)',base.meanSamples),'Power (mW)',sprintf('%s - Baseline (usb OFF)',dataset{i}));
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    subplot(nrows,ncols,2)
    plot_bar_data(u.A2mA * (u.uW2W * base.d) / volt, 1:numel(base.d),sprintf('Run (%d samples/run)',base.meanSamples),'Discharge (mAh)','Baseline (usb OFF)')
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    subplot(nrows,ncols,3)
    plot_stat_data(u.uW2mW*stdby.p',1:size(stdby.p,1), sprintf('Run (%d samples/run)',stdby.meanSamples),'Power (mW)',sprintf('%s - Standby (usb ON)',dataset{i}));
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    subplot(nrows,ncols,4)
    plot_bar_data(u.A2mA * (u.uW2W * stdby.d) / volt, 1:numel(stdby.d), sprintf('Run (%d samples/run)',stdby.meanSamples),'Discharge (mAh)','Standby (usb ON)')
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    n=2;
    for s=1:size(sizes,1)
        for f=1:numel(fps)
            subplot(nrows,ncols,n*2+1);
            plot_stat_data(u.uW2mW*tests{i}(s,f).p',1:size(tests{i}(s,f).p,1),sprintf('Run (%d samples/run)',tests{i}(s,f).meanSamples),'Power (mW)',...
                sprintf('%s %dx%d@%.2f',dataset{i},tests{i}(s,f).width,tests{i}(s,f).height,tests{i}(s,f).fps));
            set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
            subplot(nrows,ncols,n*2+2);
            plot_bar_data(u.A2mA * (u.uW2W * tests{i}(s,f).d)/volt, 1:numel(tests{i}(s,f).d), sprintf('Run (%d samples/run)',tests{i}(s,f).meanSamples),'Discharge (mAh)',...
                sprintf('%dx%d@%.2f',tests{i}(s,f).width,tests{i}(s,f).height,tests{i}(s,f).fps))
            set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
            n=n+1;
        end
    end
    
    %% 2 - plot activation data
    f2=figure('Name',[dataset{i} ' usage: runs' ]);
    set(gcf,'units','normalized','outerposition',[0 0 1 1])
    nrows=numel(fps)*size(sizes,1);
    ncols=2;
    n=0;
    for s=1:size(sizes,1)
        for f=1:numel(fps)
            subplot(nrows,ncols,n*2+1);
            plot_stat_data(tests{i}(s,f).aTime',1:size(tests{i}(s,f).aTime,1),...
                sprintf('Run (%d samples/run)',tests{i}(s,f).meanSamples),'Active time (ms)',...
                sprintf('%s %dx%d@%.2f',dataset{i},tests{i}(s,f).width,tests{i}(s,f).height,tests{i}(s,f).fps));
            
            subplot(nrows,ncols,n*2+2);
            plot_stat_data(tests{i}(s,f).sTime',1:size(tests{i}(s,f).sTime,1),...
                sprintf('Run (%d samples/run)',tests{i}(s,f).meanSamples),'Idle time (ms)',...
                sprintf('%dx%d@%.2f',tests{i}(s,f).width,tests{i}(s,f).height,tests{i}(s,f).fps));
            n=n+1;
        end
    end   
   
    %% global data - power
    f4=figure('Name',[dataset{i} ' consumption: overall (normalized)' ], 'Position',[100 100 750 400]);
    set(gcf,'units','normalized','outerposition',[0 0 1 1])
    n=1;
    for s=1:size(sizes,1)
        for f=1:numel(fps)
            subplot 121;
            l=bplot(u.uW2mW*(tests{i}(s,f).p(:)-trimmean(base.p(:),5))',n,'outliers'); hold on;
            
            subplot 122;
            l=bplot(u.uW2mW*(tests{i}(s,f).p(:)-trimmean(stdby.p(:),5))',n,'outliers'); hold on;
            n=n+1;
        end
    end
    subplot 121;
    title(sprintf('%s - Differential consumption wrt baseline \n %dx%d, %d runs, %d samples/run, %.2f secs/sample',dataset{i},tests{i}(1,1).width, tests{i}(1,1).height, tests{i}(1,1).nRun,tests{i}(1,1).meanSamples,data.tests{i}{1}.time))
    legend(l,'Location','southeast');xlabel('Framerate (fps)'); ylabel('Power (mW)');
    set(gca,'Xtick',1:numel(fps), 'XtickLabel',[cellstr(num2str(fps(1:end)', '%d'))']);
    
    subplot 122;
    title(sprintf('Differential consumption wrt Standby \n %dx%d, %d runs, %d samples/run, %.2f secs/sample',tests{i}(1,1).width, tests{i}(1,1).height,tests{i}(1,1).nRun, tests{i}(1,1).meanSamples,data.tests{i}{1}.time))
    legend(l,'Location','southeast');xlabel('Framerate (fps)'); ylabel('Power (mW)');
    set(gca,'Xtick',1:numel(fps), 'XtickLabel',[cellstr(num2str(fps(1:end)', '%d'))']);
    
    %% global data - capture time
    f5=figure('Name',[dataset{i} ' active time: overall' ], 'Position',[100 100 750 400]);
    set(gcf,'units','normalized','outerposition',[0 0 1 1])
    n=1;
    for s=1:size(sizes,1)
        for f=1:numel(fps)
            l=bplot(tests{i}(s,f).aTime(:)',n,'nooutliers');
            hold on;
            n=n+1;
        end
    end
    title(sprintf('%s - Active time for %dx%d \n (%d runs, %d samples/run each %.2f secs)',dataset{i},tests{i}(1,1).width, tests{i}(1,1).height,tests{i}(1,1).nRun, tests{i}(1,1).meanSamples,data.tests{i}{1}.time))
    legend(l(1:end-1),'Location','northeast');
    set(gca,'Xtick',1:numel(fps), 'XtickLabel',[cellstr(num2str(fps', '%d'))']);
%     set(gca,'Position',[0.075 0.1 0.925 0.85])
    xlabel('Framerate (fps)'); ylabel('Active Time (ms)')
    
    
    epsfig = hgexport('factorystyle');
    epsfig.Format = 'eps';
    hgexport(f1,sprintf('figs/SEN_1_%s_runs.eps',dataset{i}),epsfig,'Format','eps')
    hgexport(f2,sprintf('figs/SEN_2_%s_usage.eps',dataset{i}),epsfig,'Format','eps')
%     hgexport(f3,sprintf('figs/SEN_%s_consumption_overall.eps',dataset{i}),epsfig,'Format','eps')
    hgexport(f4,sprintf('figs/SEN_3_%s_consumption_overall_normalized.eps',dataset{i}),epsfig,'Format','eps')
    hgexport(f4,sprintf('figs/SEN_4_%s_capture.eps',dataset{i}),epsfig,'Format','eps')
end

%% power: baseline vs standby vs active vs idle
f10 = figure('Name',['consumption: baseline vs standby vs active vs idle' ]);
% bplot(u.uW2mW*base.p(:)',1,'nooutliers'); hold on;
Psle(1) = mean(base.p(:)); 
    
colors={'green','blue'};
for s=1:size(sizes,1)
    for f=1:numel(fps)
        for i=1:numel(dataset)            
            if(i==1)
                l=bplot(u.uW2mW*(tests{i}(s,f).p(:)-Psle)',f+1,'nooutliers');
            else
                l=bplot2(u.uW2mW*(tests{i}(s,f).p(:)-Psle)',f+1,'nooutliers');
            end
            %l=bplot(u.uW2mW*tests{i}(s,f).p(:)',f+2,'nooutliers','color',colors{i});
            hold on;
            n=n+1;
        end
    end
end
bplot(u.uW2mW*(stdby.p(:)-Psle)',1,'nooutliers'); hold on;

% axis([0.5 numel(fps)+2.5 min(u.uW2mW*base.p(:))-100 1.22e4 ])
% xlabel(sprintf('Run (%d samples/run)',base.meanSamples))
ylabel('Power (mW)');
set(gca, 'Xtick', 1:4,'XTickLabel', {'Sleep','5 fps','15fps','25fps'})
xlabel('Sensing framerate');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
%title('Consumption for USB camera (640x480) \newline Active (top)-vs-Idle (bottom) ');
% legend(l(1:end-1),'location','best')
legend('Active-median','Active-mean','Active- 25%-75%','Active- 5%-95%','Idle-median','Idle-mean','Idle- 25%-75%','Idle- 5%-95%','Location','best')
set(gca,'Position',[0.1 0.1 0.9 0.87]);

%% activation:active vs idle
f11 = figure('Name',['capture time: active vs idle (640x480)' ]);

%color={'green','blue'};
for i=1:numel(dataset)
for s=1:size(sizes,1)
        for f=1:numel(fps)            
            l=bplot(tests{i}(s,f).aTime(:)',f,'nooutliers');            
            hold on;
            n=n+1;
        end
end
end
    
% axis([0.5 numel(fps)+2.5 min(u.uW2mW*base.p(:))-100 1.22e4 ])
% xlabel(sprintf('Run (%d samples/run)',base.meanSamples))
ylabel('Time (ms)');
set(gca, 'Xtick', 1:numel(fps) ,'XTickLabel', {'5 fps','15fps','25fps'})
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
title('Capture time for USB camera (640x480) \newline Active (top)-vs-Idle (bottom)');
legend(l(1:end-1),'location','best')

%% 5 data for adjustment
%frequency-Voltage measurements 
Psle(1) = mean(stdby.p(:)*u.uW2mW);    
offset = trimmean(base.p(:),outliers);
for f=1:numel(fps)     
    Pact(1,f) = u.uW2mW*(mean(tests{1}(1,f).p(:))-offset);    
    Pact(2,f) = u.uW2mW*(median(tests{1}(1,f).p(:))-offset);
    Pact(3,f) = u.uW2mW*(trimmean(tests{1}(1,f).p(:),outliers)-offset);
    
    Pidl(1,f) = u.uW2mW*(mean(tests{2}(1,f).p(:))-offset);
    Pidl(2,f) = u.uW2mW*(median(tests{2}(1,f).p(:))-offset);
    Pidl(3,f) = u.uW2mW*(trimmean(tests{2}(1,f).p(:),outliers)-offset);
end
f12=figure('Position',[100 100 1500 400]); 
subplot 121;
plot(fps,Pact(1,:),'r+-',fps,Pact(2,:),'bx-',fps,Pact(3,:),'gs-'); hold on;
plot(fps,Pidl(1,:),'r+--',fps,Pidl(2,:),'bx--',fps,Pidl(3,:),'gs--');
legend('Active-Mean','Active-Median','Active-Mean w/o outliers',...
       'Idle-Mean','Idle-Median','Idle-Mean w/o outliers','Location','northwest');
xlabel('Framerate (fps)'); ylabel('Power (mW)');
title('SEN power consumption \newline Data for model fitting (wrt baseline power) ');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))

Psle(1) = mean(stdby.p(:)*u.uW2mW);    
offset = trimmean(stdby.p(:),outliers);
for f=1:numel(fps)     
    Pact(1,f) = u.uW2mW*(mean(tests{1}(1,f).p(:))-offset);    
    Pact(2,f) = u.uW2mW*(median(tests{1}(1,f).p(:))-offset);
    Pact(3,f) = u.uW2mW*(trimmean(tests{1}(1,f).p(:),outliers)-offset);
    
    Pidl(1,f) = u.uW2mW*(mean(tests{2}(1,f).p(:))-offset);
    Pidl(2,f) = u.uW2mW*(median(tests{2}(1,f).p(:))-offset);
    Pidl(3,f) = u.uW2mW*(trimmean(tests{2}(1,f).p(:),outliers)-offset);
end
subplot 122;
plot(fps,Pact(1,:),'r+-',fps,Pact(2,:),'bx-',fps,Pact(3,:),'gs-'); hold on;
plot(fps,Pidl(1,:),'r+--',fps,Pidl(2,:),'bx--',fps,Pidl(3,:),'gs--');
legend('Active-Mean','Active-Median','Active-Mean w/o outliers',...
       'Idle-Mean','Idle-Median','Idle-Mean w/o outliers','Location','northwest');
xlabel('Framerate (fps)'); ylabel('Power (mW)');
title('SEN power consumption \newline Data for model fitting (wrt standby power) ');
set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
%  set(gca,'Position',[0.075 0.1 0.925 0.85]);

%% fig 6 model fitting
%order of polynomial model fitting
order=[1 2 3];

sel=1;
ws = warning('off','all');  % Turn off warning
for i=1:numel(order)
    for d=1:3
        [PSen_a_p{i,d},PSena_err{i,d}] = polyfit(fps,Pact(d,:),order(i));
        [PSen_i_p{i,d},PSeni_err{i,d}] = polyfit(fps,Pidl(d,:),order(i));        
    end
end
warning (ws);  % Turn on warning

% display results
f13=figure;
set(gcf,'units','normalized','outerposition',[0 0 1 1])
label={'Mean','Median','Mean w/o outliers'};
for d=1:3
    subplot (3,2,(d-1)*2+1)    
    plot(fps,Pact(d,:),'kx','Markersize',6,'Linewidth',2); hold on;set(0, 'defaultTextInterpreter', 'tex');
    cad =[' legend (''Measurements ('  label{d} ') '''];
    for i=1:numel(order)
        pop_fit = polyval(PSen_a_p{i,d},fps,PSena_err{i,d}); plot(fps,pop_fit,color{i});  hold on;  
        cad = [cad [',''' sprintf('%d',i) '-order AvgErr=' sprintf('%.2f mW',PSena_err{i,d}.normr/numel(fps)) '''']];
    end
    eval ([cad ',''Location'', ''best'');']);
    xlabel('Framerate (fps)'); ylabel('Power (mW)');
    title(sprintf('Active power fitting - %s',label{d}))    
    axis([fps(1)-0.5 fps(end)+0.5 min(Pact(:))-100 max(Pact(:))+100])
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
    
    subplot (3,2,(d-1)*2+2)
    plot(fps,Pidl(d,:),'kx','Markersize',6,'Linewidth',2); hold on;set(0, 'defaultTextInterpreter', 'tex');
    cad =[' legend (''Measurements ('  label{d} ') '''];
    for i=1:numel(order)
        pop_fit = polyval(PSen_i_p{i,d},fps,PSeni_err{i,d}); plot(fps,pop_fit,color{i});  hold on;  
        cad = [cad [',''' sprintf('%d',i) '-order AvgErr=' sprintf('%.2f mW',PSeni_err{i,d}.normr/numel(fps)) '''']];
    end
    eval ([cad ',''Location'', ''best'');']);
    xlabel('Framerate (fps)'); ylabel('Power (mW)');
    title(sprintf('Idle power fitting - %s',label{d}))
%     set(gca, 'YTickLabel', num2str(get(gca, 'YTick'))')
    axis([fps(1)-0.5 fps(end)+0.5 min(Pidl(:))-100 max(Pidl(:))+100])
    set(gca, 'YTickLabel', num2str(get(gca, 'YTick')'))
end

hgexport(f10,sprintf('figs/SEN_0_active-idle_consumption.eps'),epsfig,'Format','eps')
hgexport(f11,sprintf('figs/SEN_0_active-idle_capture.eps'),epsfig,'Format','eps')
hgexport(f12,sprintf('figs/SEN_0_fitting_data2model.eps'),epsfig,'Format','eps')
hgexport(f13,sprintf('figs/SEN_0_fitting_results.eps'),epsfig,'Format','eps')

save('data/validation_sensing_data.mat');