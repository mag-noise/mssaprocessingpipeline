function [retInboard,retOutboard,retTime, discontinuity] = RemoveNaN(inboard, outboard, time)

    ind_in = isnan(inboard(1,:));
    ind_out = isnan(outboard(1,:));
    ind = ind_in + ind_out;
    ind = ~ind;
    retInboard = zeros(3, le);
    retOutboard = zeros(3, sum(ind));
    for i = 1:3
        disp(size( inboard(i,ind)))
        retInboard(i,ind) = retInboard(i,ind) + inboard(i,ind);
        retOutboard(i,ind) = retOutboard(i,ind) + outboard(i,ind);
    end
    
%     counter = zeros(1, length(inboard));
%     for i = 1:length(inboard)
%         counter(i) = i;
%     end
% 
%     counter = counter(ind);

    % find fcn will give the location of the next segment starting point
    % -1 makes the matrix give the end of the previous segment
    % -2 accounts for c++ array indexing
    % discontinuity = find(ischange(counter, 'linear')) - 2;

    discontinuity = find(ind) ;


    disp(length(retOutboard) == length(retInboard) && length(time) == length(retInboard))
end