function [in_result, out_result, cross_correlation, time]=test_mex(inboard, outboard, varargin)
    
    
    DefaultVcpkg = fullfile(pwd, 'tools', 'vcpkg');
    
    p = inputParser;
    checkBuild = @(x) ischar(x) && p.Results.build;
    checkAlpha = @(x) isnumeric(x) && ~(p.Results.alpha > 1 || p.Results.alpha < -1);
    checkWindow = @(x) isnumeric(x) && ~(p.Results.window > p.Results.segment);
    
    keySet = {'x', 'y', 'z'};
    xyz = containers.Map(keySet, [1,2,3]);
    checkPlot = @(x) ischar(x) && any(strcmp(keySet, x));

    addRequired(p, 'inboard', @isnumeric);
    addRequired(p, 'outboard', @isnumeric);
    addParameter(p,'build', false, @islogical);
    addParameter(p, 'vcpkgDir', DefaultVcpkg, checkBuild);
    addParameter(p, 'segment', 5000, @isnumeric);
    addParameter(p, 'window', 40, checkWindow);
    addParameter(p, 'alpha', 0.05, checkAlpha);
    addParameter(p, 'isInboard', true, @islogical);
    addParameter(p, 'xyz', 'x', checkPlot);
    addParameter(p, 'plotting', false, @islogical);

    
    parse(p, inboard, outboard, varargin{:});

    if(p.Results.build)
        build_mex(p.Results.vcpkgDir);
    end
    addpath('releases');
    mh = mexhost;
    time = clock;
    % Mex Function Input: (inboard, outboard, correlation coefficient
    % threshold)
    [in_result, out_result] = feval(mh, 'MSSAMex', inboard, outboard, ...
                            p.Results.alpha, p.Results.segment, p.Results.window);
    time = clock - time

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
    cross_correlation = xcorr(plottingBase(xyz(p.Results.xyz),:), plottingDisplay(xyz(p.Results.xyz),:));
end