function import_data(data_location)

    if(~exist('data_location', 'var'))
        data_location = "data\2016\03\11\MGF\MGF_20160311_064011_065832_v2.1.0.lv2";
    end
    if(isnumeric(data_location))
        test_location = "data\2016\03\11\MGF\";
        test_files = [
            "MGF_20160311_234614_235232_v2.1.0.lv2",
            "MGF_20160311_082211_084032_v2.1.0.lv2",
            "MGF_20160311_064011_065832_v2.1.0.lv2"
        ];
        data_location = fullfile(test_location, test_files(data_location));
    end

    % Import the file
    newData1 = importdata(data_location);
    
    % Create new variables in the base workspace from those fields.
    vars = fieldnames(newData1);
    for i = 1:length(vars)
        assignin('base', vars{i}, newData1.(vars{i}));
    end
end