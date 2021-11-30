<p align="left">
  <big>
    <b>
      <pre>
André Souza Santos                - RA: 
Carlos Eduardo Fontaneli          - RA: 
Igor Teixeira Machado             - RA: 
Júlia Aparecida Sousa de Oliveira - RA: 
Natália Bachiega Magalhães        - RA: 
Paula Vitória Larocca             - RA: 
Rafael Vinicius Polato Passador   - RA: 
Vinicius Quaresma da Luz          - RA: 
      </pre>
      <br>
      Disciplina: Sistemas Operacionais 
      <br>
      Professora: Profa. Dra. Kelen Cristiane Teixeira Vivaldini
      <br>
    </b>
  </big>
</p>

<h1 align="center"><b><big>Projeto de Implementação 01- The Roller Coaster Problem</big></b></h1>

#**1. Introdução**

<br>

Neste documento serão apresentadas as duas partes do trabalho de implementação sobre sincronização entre tarefas.
O problema escolhido pelo grupo é o "5.8 The rollercoaster problem", cuja definição é explicitada abaixo.

- Suponha que haja n passageiros e um carro de montanha-russa em um parque. Os passageiros esperam repetidamente para andar no carro, que pode acomodar no máximo C passageiros, onde C < n, no entanto, o carro pode percorrer a pista apenas quando estiver cheio. Depois de terminar um passeio, cada passageiro anda pelo parque de diversões antes de retornar à montanha-russa para andar no carro novamente. Além disso, por razões de segurança, o carro aceita apenas T embarques(respectivamente, T desembarques também) e depois encerram-se as corridas.
  <br>Sendo assim, sabe-se o seguinte:<br>
- O carro sempre anda com exatamente C passageiros;<br>
- Nenhum passageiro entrará do carro enquanto ele estiver funcionando;<br>
- Nenhum passageiro sairá no carro enquanto o carro estiver funcionando;<br>
- Nenhum passageiro solicitará outra viagem antes que possa descer do carro.<br>

<br>

Dessa forma, serão apresentados os códigos utilizados, bem como as descrições das criações destes. Ademais, apresentam-se, também, as interpretações e documentações da problemática e o funcionamento do programa baseado nas premissas de threads, controle de deadlocks e mecanismos de sincronização.

Para o código funcionar no Collab, uma plataforma ".ipynb", é necessário executar o código e, logo em seguida, executar o "script bash".

#**2. Interpretação do Problema**

##**2.1. Primeira Solução**

<br>

Na perspectiva do grupo, compreendemos que o número total de passeios que o carrinho da montanha-russa pode realizar não é baseado no número total de pessoas no parque, considerando um carrinho que comporta um tamanho C, mas sim em um número pré-definido de corridas.
<br><br>
Assim, para implementação prática do problema, utilizamos o conceito de threads. Cada passageiro é representado por uma thread no programa, implementada a partir da função `pthread_create()`. Em diversas situações, threads são independentes, contudo, nesse problema de sincronização, faz-se necessário garantir que cada thread possa compartilhar dados e se preocupar com o que as outras threads estão fazendo, uma vez que, por exemplo, um passageiro só pode embarcar em um passeio caso o carrinho possuir vaga para ele.
<br><br>
Na primeira solução, utilizamos a estratégia de sincronização com semáforos. Essa técnica consiste na realização de duas operações: uma que
incrementa e outra que decrementa o seu valor. Durante a verificação ou alteração do valor de um semáforo um processo não pode ser escalonado e é por isso que essa variável nos ajuda a resolver a problemática. Como há um limite de passageiros por carrinho e, ao mesmo tempo, as operações de embarque e desembarque devem ser realizadas de maneira que apenas uma pessoa acione-as por vez, os semáforos controlam o número máximo de acessos simultâneos a um recurso, neste caso o acesso ao carro.
<br><br>
Dessa maneira, antes de entrar em uma região crítica, o processo verifica o valor do semáforo e, quando o valor é 0, ele não pode prosseguir. Assim, torna-se possível controlar o número máximo de passageiros por viagem, bem como o embarque e o desembarque dos mesmos.
<br><br>
Em contrapartida, é possível analisar que, caso não hajam passageiros suficientes para embarque no carrinho, o qual só sairá para passeio na condição de estar totalmente ocupado, ocorrerá um problema de deadlock - uma situação em que ocorre um impasse e dois ou mais processos ficam impedidos de continuar suas execuções, ou seja, ficam bloqueados. Outrossim, alocamos cuidadosamente os recursos para que os processos não entrem em condições das quais não podem sair, limitando o número de passeios baseado previamente. Tal escolha, torna possível que o carro não fique estagnado, mas, por outro lado, também faz com que alguns passageiros acabem indo mais de uma vez na montanha russa e que não haja garantia de que todos os passageiros passeiem a mesma quantidade de vezes. <br><br>
Para assegurar que as viagens se iniciem e terminem corretamente, utilizamos a função `sleep()`, combinada com o `srandomtime()`, em que o tempo de espera representa o passeio na montanha-russa. Haja vista que não há prioridade entre os passageiros, não há prioridade entre os processos e, em decorrência disso, não lidamos com situações de ''starvation'', nos quais processos de alta prioridade continuam em execução e processos de baixa prioridade são bloqueados.
<br><br>
Por fim, observa-se também a utilização de e `onBoard`(passageiros que embarcaram). Como na definição da problemática consta que, assegurar que os passageiros só embarquem enquanto a corrida não estiver começado e só desembarquem após o término do passeio, utilizamos essa variável global que atua como uma "catraca" e assegura as regras da montanha-russa, liberando a corrida apenas quando o carro estiver cheio(quando `onBoard` se iguala a `CAR_CAPACITY`). Em tal contexto, utilizou-se `MAX_RIDES` como o limitador de corridas, fazendo com o loop das corridas fosse limitado por tal valor.

##**2.2. Segunda Solução**
<br>

Paralelamente, na segunda solução proposta pelo grupo, optamos em utilizar barreiras para cumprir os desafios da implementação da problemática. Essa técnica de sincronização consiste em: fazer com que diversas threads esperem até que todas as threads tenham alcançado um determinado ponto de execução antes de que qualquer outra thread continue.
<br><br>

Nesse ínterim, no contexto da montanha-russa, a barreira serve como a "catraca" que permite o embarque e o desembarque no carrinho. Dessa forma, defininindo um número C de passegeiros por carrinho, cria-se uma barreira de mesmo tamanho, para que seja possível controlar esse limite necessário para iniciar a corrida.
<br><br>
Contudo, somente tal condição não é suficiente para suprir a sincronização das threads. Haja vista que todas as threads são criadas de maneira conjunta, no mesmo laço de maneira sequêncial, elas atingem sincronizadamente a barreira de maneira simultânea, resultando em uma sáida indesejada no programa que demonstra desrespeitar a condição necessária das threads que ultrapassaram a barreira sejam finalizadas antes de que outras consigam ultrapassar também.
<br><br>
Dessa maneira, nas situações em que o número de passageiros por corrida difere do número de passageiros no parque, a saída do programa diverge da esperada, em decorrência do entrave supracitado. Sendo assim, utilizamos semáforos para garantir um embarque e um desembarque que respeite as condições necessárias. Assim, o semáforo garante que a "catraca" do carrinho funcione corretamente, impedindo que outras threads acionem as funções load() e unload() antés de que o bloqueio seja liberado, fazendo com que essas situações ocorram somente quando o contador do semáforo seja nulo.
<br><br>
No final, temos então o laço de repetição no carrinho, com critério de parada baseado no número total de corridas que ele tem permissão para realizar. Vale lembrar que as barreiras tem tamanho (CAR_CAPACITY + 1) pois, além das threads que representam os passageiros, a thread do carrinho também atinge as barreiras.

#**3. Problemas Encontrados**
<br>

O maior desafio enfretado pelo grupo foi garantir o embarque e o desembarque de C passageiros atendendo as condições de: permitir o embarque, se e somente se, o carrinho estiver vazio e a corrida não iniciada, e o desembarque, quando o carrinho estiver cheio e a corrida já finalizada.
<br><br>
Na primeira solução de implementação, utilizando somente semáforos, precisamos da utilização de contadores globais que asseguram a quantidade correta de passageiros no carrinho, bem como liberam a possibilidade de embarque e desembarque. Concomitantemente, na segunda solução, enfrentamos o problema de todas as threads, que foram criadas juntas, acionarem a barreira ao mesmo tempo, inviabilizando uma saída adequada do programa.
<br><br>
Além disso, na primeira solução, inicialmente o grupo tentou utilizar somente 2 semáforos, um para os passageiros subirem e outro para os passageiros descerem. Entretanto, essa técnica causou um problema de compartilhamento de recurso, pois, assim que o semáforo de desembarque é acionado, outros passageiros, que já estavam prontos para embarque, acabam embarcando, acarretando em uma desincronização de etapas, pois não havia ocorrido o desembarque por completo. Em decorrência disso, foi necessário usar dois semáforos adicionais para que se fizesse o controle de recursos(embarque e desembarque), garantindo as etapas fossem concluídas por todos os passageiros para então, somente após o embarque e desembarque completo do carro, o recurso ser liberado.
<br><br>
Conjuntamente, na solução usando barreiras, as threads criadas atingiam a barreira de maneira quase simultânea, de forma que, em alguns casos, todas passavam ao mesmo tempo, corrompendo a saída. Para resolver esse entrave, combinamos as barreiras com dois semáforos de embarque e desembarque que asseguram o comportamento correto.
<br><br>
Não obstante, é necessário salientar que a utilização de semáforos pode acarretar em deadlocks. Quando os semáforos são inicializados de maneira errônea, as threads não conseguem sair ou entrar da região crítica, sendo impedidos de continuar suas execuções e interrompendo o programa. Quanto ao problemas de "starvation", como nossas soluções não abordam situações de prioridade, uma vez que o enunciado do problema não define nem essa prioridade, nem que todos as pessoas no parque precisam efetivamente correr na montanha-russa.

#**4. Considerações Finais**
<br>

A partir da atividade de implementação prática do problema conhecido como "Roller Coaster Problem", foi possível entender, de maneira plena, o funcionamento de threads e como sincronizá-las de maneira que seja possível assegurar o compartilhamento efetivo de recursos - evitando problemas de "deadlock" e "starvation".
<br><br>
Nesse contexto, por mais que ocorreram alguns problemas de sincronização mediante o embarque e desembarque(liberação parcial de recurso), foi exequível realizar duas soluções diferentes que solucionam o entrave de acordo com a interpretação do grupo mediante a problemática.
<br><br>
Por fim, ressaltou-se a importância da definição prévia das condições para acesso aos recursos necessários e, por conseguinte, quais devem ser as limitações que assegurarão tais condições. Em nosso caso, essa perspectiva é vista na questão do acesso ao embarque, corrida e desembarque. Para haver embarque: deve ser garantido que exatamente todos os passageiros, que haviam embarcados, tenham desembarcado do carro e/ou que este, por sua vez, esteja vazio; para haver corrida: deve se embarcar a exata capacidade total do carro; para haver desembarque: a corrida deve ser finalizada por completo.
<br><br>
