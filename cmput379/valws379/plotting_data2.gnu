
# Plotting the data of file plotdata2.dat


# wxt
set terminal wxt size 600,480 enhanced font 'Verdana,10' persist

unset key
set title "frequence histogram"
set xlabel "value"
set ylabel "occurance" 
set style data histogram
set style fill solid border
plot 'plotdata2.dat' using 2
