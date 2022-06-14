function signal = recon(input_signal, interference)
    
    count = 0;
    signal = zeros(1, length(input_signal));
    for i = 1:size(input_signal, 1)
        alpha = corrcoef(input_signal(i,:), interference);
        if( alpha(2) > 0.05 )
            signal = signal + input_signal(i, :);
            count = count + 1;
        end
    end
    disp(count);
end