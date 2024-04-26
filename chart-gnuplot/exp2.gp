set terminal pdf color enhanced font 'Calibri,16' size 14cm,10cm
set output 'graph2.pdf'
set key inside left top font 'Calibri,16'
set colorsequence podo
set style line 1 lc rgb 'blue' lw 1 pt 5 ps 0.5
set style line 2 lt 1 lw 2 pt 2 ps 0.5
set style line 3 lt 2 lw 2 pt 3 ps 0.5

set xlabel "n" font 'Calibri,16'
set ylabel "S" font 'Calibri,16'
set format y "%.1f"
set format x "%.0f"
set xtics 2,1,4

plot x title "Linear speedup" with lines lc rgb 'blue' lt 1 lw 2,\
     'mntkrl10^7.txt' using 1:2 title "N = 10^7" with linespoints ls 2,\
     'mntkrl10^8.txt' using 1:2 title "N = 10^8" with linespoints ls 3