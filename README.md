# if669-jogo
[Dcto](https://docs.google.com/document/d/1GUkAA6Tv_SgV5HuBIBtLkdRdiGoB3Gxj1L-__42aXpQ/edit)

## Estrutura
A idéia é desenvolver algumas partes da funcionalidade separadamente, ou experimentar e depois juntar-las no projeto principal.
Essas partes separadas estão na pasta _exemplos_

## Comunicação
Por em quanto, o servidor mantém um vetor com os estados de todos os jogadores, (teclado, posição, etc) e recebe um byte de input dos
jogadores, quando eles pressionam alguma tecla. 

Este byte encoda qual tecla esta sendo pressionada, se o evento foi de pressionamento 
ou soltura da tecla e um bit para checar se a mensagem é valida.

A cada ciclo o servidor deverá fazer o broadcast do vetor com os estados para todos os jogadores, ~(a frequencia disso ainda sera 
definida e implementada)~.

## Lógica do jogo
A estrutura interna do jogo será uma matriz e mais umas coisas ai.


## TODO
~O broadcast ainda não funciona bem, tão saindo alguns bytes certos, mas é preciso debugar isso: criando um ponteiro de char que aponta
para o inicio do vetor de estado e percorrendo ele até o fim, para analizar os dados.~
