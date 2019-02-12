%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% How to use:
%%    > file_name -> file's name where is stored the data 
%%    > version -> 1 if the data was obtained using the normal brute force algorithm; 
%%                 2 if the data was obtained using the fastest brute force algorithm; 
%%    > make_approximation_until_max_n -> boolean value: true if user wants just to obtain approximation function's plot
%%                                                       false if user wants to obtain the plot of data collected alongside approximation function's plot
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function make_plots(file_name, version, make_approximation_until_max_n)
   %%%%%%%%%%%%%%%%%% constants %%%%%%%%%%%%%%%%%%
   LINE_WIDTH = 3;
   DOT_WIDTH = 10;
   FIGURE_DIMENSIONS = [0 0 1920 1080];
   PLOT_TITLE_SIZE = 22;
   PLOT_LABELS_SIZE = 20;
   PLOT_VALUES_SIZE = 20;
   PLOT_LEGEND_SIZE = 20;
   PLOT_LEGEND_LOCATION = 'northwest';
   PLOT_TITLES = "Running time plot for";
   PLOT_TITLES_VERSION = ["normal brute force algorithm"
                          "fastest brute force algorithm"];
   PLOT_XLABELS = "n";
   PLOT_YLABELS = "Running time (s)";
   PLOT_COLORS = [[30, 144, 255], 
                  [255, 69, 0]]/255;

   PNG_NAMES_VERSION = ["nb", "fb"];
   PNG_NAMES_APPROXIMATION = ["approximation", "real"];

   MAX_N = 32;
   ALL_M = [[10^-6: 10^-10: 10^-5], 
            [10^-7: 10^-11: 10^-6]];


   %%%%%%%%%%%%%%%%%% work with data %%%%%%%%%%%%%%%%%%
   data_file = fopen(file_name, 'r');
   data = fscanf(data_file, '%f', [10, Inf]);
   fclose(data_file);

   png_file_name = sprintf('%s_%s', ...
                           PNG_NAMES_VERSION(version));

   n = data(1, :);
   n_approximation_function = n;
   time_seconds = data(10, :);
   time_seconds_estimation = -time_seconds;
   m = 0;   %approximation function 'slope' 
   
   %approximation function for brute force
   for i = ALL_M(version, :)
      new_estimation = (2.^n)*i;
      if sum(abs(new_estimation - time_seconds)) < sum(abs(time_seconds_estimation - time_seconds))
         time_seconds_estimation = new_estimation;
         m = i;
      end
   end

   %time plot
   figure('Position', FIGURE_DIMENSIONS);
   legend_text = cell(1, 2);
   legend_text{2} = sprintf('O(%d*2^n)', m);

   if ~make_approximation_until_max_n
      plot(n, time_seconds, '-o', ...
           'LineWidth', LINE_WIDTH, ...
           'MarkerSize', DOT_WIDTH, ...
           'color', PLOT_COLORS(1, :));
      hold on;
      legend_text{1} = 'Running time';

      png_file_name = sprintf('%s_%s', ...
                              png_file_name, ...
                              PNG_NAMES_APPROXIMATION(2));
   else
      n_approximation_function = [n(1):MAX_N];
      time_seconds_estimation = (2.^n_approximation_function)*m;

      legend_text{1} = legend_text{2};
      legend_text(end) = [];

      png_file_name = sprintf('%s_%s', ...
                              png_file_name, ...
                              PNG_NAMES_APPROXIMATION(1));
   end

   plot(n_approximation_function, time_seconds_estimation, '-o', ...
        'LineWidth', LINE_WIDTH, ...
        'MarkerSize', DOT_WIDTH, ...
        'color', PLOT_COLORS(2, :));
   legend(legend_text, ...
          'FontSize', PLOT_LEGEND_SIZE, ...
          'Location', PLOT_LEGEND_LOCATION);
   
   title(sprintf('%s %s', PLOT_TITLES, PLOT_TITLES_VERSION(version)), ...
         'FontSize', PLOT_TITLE_SIZE);
   xlabel(PLOT_XLABELS, ...
          'FontSize', PLOT_LABELS_SIZE);
   ylabel(PLOT_YLABELS, ...
          'FontSize', PLOT_LABELS_SIZE);
   set(gca, 'FontSize', PLOT_VALUES_SIZE);
   
   grid on;
   grid minor;

   print(png_file_name,'-dpng','-r0'); %Generates automaticaly a png image from displayed figure
end
