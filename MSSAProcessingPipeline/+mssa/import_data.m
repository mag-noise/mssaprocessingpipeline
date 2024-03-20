function import_data(data_location, debug, only_path)
    % Dev tool to import data from data folder.
    % Not particularly useful outside of development

    if(~exist('data_location', 'var'))
        data_location = "data\2016\03\11\MGF\MGF_20160311_064011_065832_v2.1.0.lv2";
    end
    if(~exist('debug', 'var'))
        debug = -1;
    end
    if(~exist('only_path', 'var'))
        only_path = false;
    end
    iscdf = false;

    if(~only_path)
        if(isnumeric(data_location))
            if(data_location > 3)
                test_location = dir(fullfile("data", "*.cdf"));
                test_files = char(test_location.name);
                data_location = data_location - 3;
                test_location = "data";
                iscdf = true;
            else
                test_location = fullfile("data","2016","03","11","MGF");
                test_files = [
                    "MGF_20160311_234614_235232_v2.1.0.lv2",
                    "MGF_20160311_082211_084032_v2.1.0.lv2",
                    "MGF_20160311_064011_065832_v2.1.0.lv2"
                ];
    
            end
            data_location = fullfile(test_location, test_files(data_location, :));
        end
    
        if(~iscdf)
            % Import the file
            newData1 = importdata(data_location);
            
            % Create new variables in the base workspace from those fields.
            vars = fieldnames(newData1);
            for i = 1:length(vars)
                assignin('base', vars{i}, newData1.(vars{i}));
            end
        
    
    
        else
            mssa.format_cdf(data_location);
        end
    end
    if(debug > -1)
        if(debug)
            rmpath(fullfile("build","x64-Release", "mssa"))
            addpath(fullfile("build","x64-Debug", "mssa"))
        else
            rmpath(fullfile("build","x64-Debug", "mssa"))
            addpath(fullfile("build","x64-Release", "mssa"))
        end
    end
end