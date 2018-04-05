
# Plotting the data of file plotdata1.dat


# wxt
set terminal wxt size 600,480 enhanced font 'Verdana,10' persist

unset key
set title "time series"
set xlabel "time"
set ylabel "value"
plot 'plotdata1.dat' smooth csplines
