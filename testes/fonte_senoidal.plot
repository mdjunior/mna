set title  "Fonte senoidal"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/fonte_senoidal.net.1512848640.png"
plot 'testes/fonte_senoidal.net.1512848640.tsv' using 1:2 title 'saida fonte'
