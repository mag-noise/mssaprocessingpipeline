build_mex;
% Import the file
newData1 = importdata("data\2016\03\11\MGF\MGF_20160311_064011_065832_v2.1.0.lv2");

% Create new variables in the base workspace from those fields.
vars = fieldnames(newData1);
for i = 1:length(vars)
    assignin('base', vars{i}, newData1.(vars{i}));
end

mh = mexhost;
[in_result, out_result] = feval(mh, 'mexEntry', B_Inboard, B_Outboard);
plot(B_Inboard(1,:))
hold on
plot(in_result(1,:))
hold off