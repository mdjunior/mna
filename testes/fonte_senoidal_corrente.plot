set title  "Fonte senoidal de corrente"
set xlabel "tempo"
set ylabel "corrente"
set term png size 1024,768
set output "testes/fonte_senoidal_corrente.net.1481813862.png"
plot 'testes/fonte_senoidal_corrente.net.1481813862.tsv' using 1:2 title 'fonte'