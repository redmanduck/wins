A = csvread('map_AB_raw.csv');
B = csvread('map_C_raw.csv');
B(:,2) = B(:,2) + 15
A(:,2) = A(:,2) + 15
% concatenate both files
Cat = [A;B];

% find ranges
maxrange = max(Cat);
maxrange_x = maxrange(1)
maxrange_y = maxrange(2)

% normalize them to 128x64
Cat(:,1) = ceil(Cat(:,1)*100 ./ maxrange_x) + 10
Cat(:,2) = ceil(Cat(:,2)*50 ./ maxrange_y) + 10

%plot them
col1 = Cat(:, 1);
col2 = Cat(:, 2);
scatter(col1, col2,'bX')

axis([0 128 0 64])

csvwrite('map_norm.csv', Cat)