# Projeto Final
Para compilar o codigo digite make no terminal

Para executar digite make run

Para limpar os arquivos digite make clean

Para executar a versão serial digite ./SERIAL $timeout $k $arquivo_de_entrada
A saída é um arquivo contendo a k-plex encontrada escrito em output/$arquivo_de_entrada

Para executar a versão com balanceamento de carga digite ./LOAD $timeout $numero_de_threads $k $arquivo_de_entrada
A saída é um arquivo contendo a k-plex encontrada escrito em output/$arquivo_de_entrada

Para executar a versão com uma pilha de tarefas digite ./LOAD $timeout $numero_de_threads $k $arquivo_de_entrada
A saída é um arquivo contendo a k-plex encontrada escrito em output/$arquivo_de_entrada