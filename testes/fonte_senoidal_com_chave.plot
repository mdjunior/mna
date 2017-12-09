set title  "Fonte senoidal"
set xlabel "tempo"
set ylabel "tensão na chave"
set term png size 1024,768
set output "testes/fonte_senoidal_com_chave.net.1512857720.png"
plot 'testes/fonte_senoidal_com_chave.net.1512857720.tsv' using 1:3 title 'tensao na chave'
