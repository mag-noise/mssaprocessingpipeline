function [retInboard,retOutboard,retTime, discontinuity] = RemoveNaN(inboard, outboard, time)

    ind_in = isnan(inboard(1,:));
    ind_out = isnan(outboard(1,:));
    ind = ind_in + ind_out;
    ind = ~ind;
    retInboard = zeros(3, sum(ind));
    retOutboard = zeros(3, sum(ind));
    for i = 1:3
        disp(size( inboard(i,ind)))
        retInboard(i,:) = retInboard(i,:) + inboard(i,ind);
        retOutboard(i,:) = retOutboard(i,:) + outboard(i,ind);
    end
    
    counter = zeros(1, length(inboard));
    for i = 1:length(inboard)
        counter(i) = i;
    end

    counter = counter(ind);
    discontinuity = find(ischange(counter, 'linear'));



    retTime = time(ind);

    disp(length(retOutboard) == length(retInboard) && length(time) == length(retInboard))
end