set title  "Fonte DC"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/fonte_dc.net.1512882053.png"
plot 'testes/fonte_dc.net.1512882053.tsv' using 1:2 title 'saida fonte'
