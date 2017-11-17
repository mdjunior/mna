set title  "Amplificador inversor com fonte DC"
set xlabel "tempo"
set ylabel "tensão"
set term png size 1024,768
set output "testes/amplificador_operacional_inversor.net.1481771489.png"
plot 'testes/amplificador_operacional_inversor.net.1481771489.tsv' using 1:4 title 'tensao na saida do ampop', 'testes/amplificador_operacional_inversor.net.1481771489.tsv' using 1:2 title 'tensao da fonte'
