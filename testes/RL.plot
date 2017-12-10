set title  "Circuito RL"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/RL.net.1512884778.png"
plot 'testes/RL.net.1512884778.tsv' using 1:2 title 'fonte', 'testes/RL.net.1512884778.tsv' using 1:3 title 'indutor'
