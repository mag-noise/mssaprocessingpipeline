%%%%%%%%%%%%%%%%%%%%%
% Function to load a CDF into a usable format by MSSAMex
% Parameters:
%   -data_location: path string to CDF to load
function format_cdf(data_location)

    cdfid = cdflib.open(data_location);
    in_num = cdflib.getVarNum(cdfid, 'B_inboard_CRF');
    out_num = cdflib.getVarNum(cdfid, 'B_outboard_CRF');
    time_num = cdflib.getVarNum(cdfid, 'Timestamp');

    maxRecNum = cdflib.getVarMaxWrittenRecNum(cdfid, in_num);
    data= cdflib.hyperGetVarData(cdfid,  in_num, [0 maxRecNum 1], {0 3 1});
    assignin('base', "inboard", data);

    maxRecNum = cdflib.getVarMaxWrittenRecNum(cdfid, out_num);
    data= cdflib.hyperGetVarData(cdfid,  out_num, [0 maxRecNum 1], {0 3 1});
    assignin('base', "outboard", data);

    maxRecNum = cdflib.getVarMaxWrittenRecNum(cdfid, time_num);
    data= cdflib.hyperGetVarData(cdfid,  time_num, [0 maxRecNum 1]);
    assignin('base', "timestamp", data);
end