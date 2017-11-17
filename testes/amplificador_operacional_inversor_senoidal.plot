set title  "Amplificador inversor com fonte senoidal"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/amplificador_operacional_inversor_senoidal.net.1481773626.png"
plot 'testes/amplificador_operacional_inversor_senoidal.net.1481773626.tsv' using 1:2 title 'fonte',  'testes/amplificador_operacional_inversor_senoidal.net.1481773626.tsv' using 1:4 title 'saida invertida'
