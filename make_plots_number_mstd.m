clear all;

LINE_WIDTH = 3;
DOT_WIDTH = 10;
PLOT_TITLE_SIZE = 20;
PLOT_LABELS_SIZE = 18;
PLOT_VALUES_SIZE = 18;
PLOT_LEGEND_SIZE = 18;
PLOT_LEGEND_LOCATION = 'northwest';


file_name = 'data_fb.txt';

data_file = fopen(file_name, 'r');
data = fscanf(data_file, '%f', [10, Inf]);
fclose(data_file);

n = data(1, :);

%number of sets plot
figure('Position', [0 0 1920 1080]);
plot(n, data(8, :), '-o', ...
     'LineWidth', LINE_WIDTH, ...
     'MarkerSize', DOT_WIDTH);
title('Number of A sets where |A+A|>|A-A|', ...
      'FontSize', PLOT_TITLE_SIZE);
xlabel('n', ...
       'FontSize', PLOT_LABELS_SIZE);
ylabel('Number of A sets', ...
       'FontSize', PLOT_LABELS_SIZE);
set(gca, 'FontSize', PLOT_VALUES_SIZE);
grid on;
grid minor;

%proportions plot
figure('Position', [0 0 1920 1080]);
plot(n, data(9, :), '-o', ...
     'LineWidth', LINE_WIDTH, ...
     'MarkerSize', DOT_WIDTH);
title('Proportion between the number of A sets where |A+A|>|A-A| and the total number of sets', ...
      'FontSize', PLOT_TITLE_SIZE);
xlabel('n', ...
       'FontSize', PLOT_LABELS_SIZE);
ylabel('Proportion   ', ...
       'FontSize', PLOT_LABELS_SIZE);
set(gca, 'FontSize', PLOT_VALUES_SIZE);
grid on;
grid minor;

%logarithm
log_funct = log2(data(8, :));
log_funct = log_funct(n >= 15);
n = n(n >= 15);

m = (log_funct(n == 32) - log_funct(n == 18))/(32 - 18);
b = 0;
approximation_funct = m*n + b;

for i = [-100: 0.01: 0]
       new_estimation = m*n + i;
       if sum(abs(new_estimation - log_funct)) < sum(abs(approximation_funct - log_funct))
              approximation_funct = new_estimation;
              b = i;
       end
end

figure('Position', [0 0 1920 1080]);
plot(n, log_funct, '-o', ...
     'LineWidth', LINE_WIDTH, ...
     'MarkerSize', DOT_WIDTH);
hold on;
plot(n, approximation_funct, '-o', ...
     'LineWidth', LINE_WIDTH, ...
     'MarkerSize', DOT_WIDTH);
title('Logarithm of number of A sets where |A+A|>|A-A|', ...
      'FontSize', PLOT_TITLE_SIZE);
xlabel('n', ...
       'FontSize', PLOT_LABELS_SIZE);
ylabel('Number of A sets', ...
       'FontSize', PLOT_LABELS_SIZE);
set(gca, 'FontSize', PLOT_VALUES_SIZE);
legend({'log_2(number of A sets)', sprintf('%d*n + (%d)', m, b)}, ...
       'FontSize', PLOT_LEGEND_SIZE, ...
       'Location', PLOT_LEGEND_LOCATION);
grid on;
grid minor;