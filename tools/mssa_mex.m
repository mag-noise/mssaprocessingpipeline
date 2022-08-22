% Function to provide interface for MSSAMex.
% Parameters:
%   -inboard: 3xN D array of magnetic field data
%   -outboard: 3xN D array of magnetic field data
%   -timestamp: datetime or datenum corresponding to inboard and outboard
%   -build: compile and build MSSAMex inside matlab (not recommended)
%   -vcpkgDir: location of vcpkg root directory
%   -segment: Input segmentation size for MSSA
%   -window: MSSA window size
%   -alpha: Threshold value for MSSA component selection
%   -isInboard: Used for display selection purposes
%   -xyz: char value x, y, or z to select which component to plot
%   -plotting: logical value to decide whether or not to plot
function [in_result, out_result, flags, in_wheel, out_wheel, time]=mssa_mex(inboard, outboard, timestamp, varargin)    
    
    DefaultVcpkg = fullfile(pwd, 'tools', 'vcpkg');
    
    p = inputParser;
    checkBuild = @(x) ischar(x) && p.Results.build;
    checkAlpha = @(x) isnumeric(x) && ~(p.Results.alpha > 1 || p.Results.alpha < -1);
    checkWindow = @(x) isnumeric(x) && ~(p.Results.window > p.Results.segment);
    checkTime = @(x) isdatetime(x) || isnumeric(x);

    keySet = {'x', 'y', 'z'};
    xyz = containers.Map(keySet, [1,2,3]);
    checkPlot = @(x) ischar(x) && any(strcmp(keySet, x));

    addRequired(p, 'inboard', @isnumeric);
    addRequired(p, 'outboard', @isnumeric);
    addRequired(p, 'timestamp', checkTime);
    addParameter(p,'build', false, @islogical);
    addParameter(p, 'vcpkgDir', DefaultVcpkg, checkBuild);
    addParameter(p, 'segment', 5000, @isnumeric);
    addParameter(p, 'window', 40, checkWindow);
    addParameter(p, 'alpha', 0.05, checkAlpha);
    addParameter(p, 'isInboard', true, @islogical);
    addParameter(p, 'xyz', 'x', checkPlot);
    addParameter(p, 'plotting', false, @islogical);

    
    parse(p, inboard, outboard, timestamp, varargin{:});

    if(p.Results.build)
        build_mex(p.Results.vcpkgDir);
    end
    %addpath('release/mssa');
    mh = mexhost;
    time = clock;
    % Mex Function Input: (inboard, outboard, correlation coefficient
    % threshold, segment size, window size)
    [in_result, out_result, flags, in_wheel, out_wheel] = ...
        feval(mh, 'MSSAMex', inboard, outboard, timestamp, ...
        p.Results.alpha, p.Results.segment, p.Results.window);

    time = clock - time;
    disp("Time:");
    disp(time);

    plottingDisplay = in_result;
    plottingBase = inboard;

    if(~p.Results.isInboard)
        plottingDisplay = out_result;
        plottingBase = outboard;
    end
    if(p.Results.plotting)
        plot(plottingBase(xyz(p.Results.xyz),:))
        hold on
        plot(plottingDisplay(xyz(p.Results.xyz),:))
        legend('Original', 'Recreation')
        hold off
    end
    %cross_correlation = xcorr(plottingBase(xyz(p.Results.xyz),:), plottingDisplay(xyz(p.Results.xyz),:));
end