# Atividade 28-04

Este é o repositório que armazena a tarefa solicitada no dia 22/04. Todos os arquivos necessários à execução já foram criados, de modo que basta seguir as instruções abaixo para executá-lo em seu dispositivo.

## Como Usar

1. Para acessar a atividade aramzenada, clone este repositório para seu dispositivo executando o seguinte comando no terminal de um ambiente adequado, como o VS Code, após criar um repositório local: 
git clone https://github.com/nivaldojunior037/Atividade-28-04-EmbarcaTech

2. Após isso, importe como um projeto a pasta que contém os arquivos clonados em um ambiente como o VS Code e compile o código existente para que os demais arquivos necessários sejam criados em seu dispositivo

3. Ao fim da compilação, será gerado um arquivo .uf2 na pasta build do seu repositório. Esse arquivo deve ser copiado para a BitDogLab em modo BOOTSEL para que ele seja corretamente executado. 

4. É necessário conectar um resistor de 10k ohms ao terminal de tensão 3,3V e um resistor desconhecido ao Ground da placa. A porta GPIO 28 deve ser conectada ao nó entre o resistor conhecido e o desconhecido.

### Como Executar o Código

1. Para executar o código, basta manter a conexão dos resistores e dos terminais da BitDogLab enquanto se desejar realizar a medição. O valor do resistor desconhecido será mostrado no display da placa junto aos valores das faixas calculadas.

2. Na matriz de LEDs será mostrado, por meio de código de cores, o valor as faixas do resistor conectado. Cada representação significa um número, havendo o caso especial do 0, que é representado por uma coluna de LEDs com 3 acesos e 2 apagados. Os demais buscam representar de forma mais aproximada o código real de cores de resistores. 

#### Link do vídeo

Segue o link do Drive com o vídeo onde é demonstrada a utilização do código: https://drive.google.com/file/d/10TjMnjXujsJuRR5qOfYxVEn6CcEoTFys/view?usp=sharing 