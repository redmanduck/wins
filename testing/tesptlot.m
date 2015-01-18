for n = 0:49
    M = csvread(strcat('discrete.txt_processed_', num2str(n), '.csv'));
    plot(M,'.');
    axis([-5 10 0 100])
    hold on
end