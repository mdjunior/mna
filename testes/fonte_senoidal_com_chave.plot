set title  "Fonte senoidal com chave"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/fonte_senoidal_com_chave.net.1512857720.png"
plot 'testes/fonte_senoidal_com_chave.net.1512857720.tsv' using 1:2 title 'tensao na fonte', 'testes/fonte_senoidal_com_chave.net.1512857720.tsv' using 1:3 title 'tensao na chave'
