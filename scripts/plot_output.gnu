datafile = 'C:\Local\Eclipse\workspace\SimpleBuildingSimulator\output\simulation.csv'

set terminal wxt size 1300, 800
set multiplot layout 3, 1

set key outside

set datafile separator comma
set key autotitle columnhead

set y2range [-1:2]
set y2tics border
set title "Room Temperature and SPOT Status"
set ylabel "Temperature (^{\260}C)"
set y2label "SPOT State [0/1]"
plot datafile \
	using 0:1 with linespoints pt 7 lw 2 axes x1y2, \
	'' using 0:4 with linespoints pt 5 lw 2, \
	'' using 0:5 with linespoints pt 2 lw 2
	
set title "Weather and Occupancy (Room)"
set y2range [-1:1]
set y2tics border
set ylabel "Temperature (^{\260}C)"
set y2label "Occupancy [0/1]"
plot datafile \
	using 0:2 with linespoints pt 7 lw 2 axes x1y2, \
	'' using 0:10 with linespoints pt 5 lw 2
	
set title "PPV, Mixed Air Temperature and AHU Power"
set y2tics border
unset y2range
set ylabel "PPV and P_{total} (kW)"
set y2label "Mixed Air Temperature (^{\260}C)"
plot datafile \
	using 0:8 with linespoints pt 7 lw 2, \
	'' using 0:9 with linespoints pt 5 lw 2 axes x1y2, \
	'' using 0:11 with linespoints pt 3 lw 2 title "P_{total}"

unset multiplot
pause mouse keypress '\n'