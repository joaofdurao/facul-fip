# Conversor de Bases Numéricas

Este projeto é um conversor de números entre as bases decimal, binária, octal e hexadecimal.
Ele também guarda um histórico de cada conversão em um arquivo de texto, para que você possa ver o que já foi feito.

## O que este programa faz

O programa mostra um menu com opções para:

1. Converter decimal para binário
2. Converter decimal para hexadecimal
3. Converter decimal para octal
4. Converter binário para decimal
5. Converter hexadecimal para decimal
6. Ver o histórico de conversões
7. Ver estatísticas de uso
8. Sair

## Como funciona o fluxo

1. Compile o arquivo `conversor.c` com o comando:

   ```bash
   gcc -Wall -Wextra -o conversor conversor.c
   ```

2. Execute o programa com:

   ```bash
   ./conversor
   ```

3. Quando o menu aparecer, escolha uma opção digitando o número correspondente.

4. Para converter um número, digite o valor solicitado e pressione Enter.

5. O resultado é mostrado imediatamente na tela.

6. Cada conversão é registrada em `historico_conversoes.txt` com data e hora.

7. Use a opção 6 para ler todo o histórico de conversões já feitas.

8. Use a opção 7 para ver estatísticas como:
   - quantas conversões já foram feitas
   - qual tipo de conversão é o mais comum

9. Use a opção 8 para fechar o programa.

## Detalhes importantes para leigos

- "Base decimal" é o sistema de numeração comum, que usa os dígitos 0 a 9.
- "Base binária" usa apenas 0 e 1.
- "Base octal" usa os dígitos 0 a 7.
- "Base hexadecimal" usa 0 a 9 e A a F.

## Arquivo de histórico

O arquivo `historico_conversoes.txt` é criado automaticamente quando você faz a primeira conversão.
Ele agora começa com uma linha de cabeçalho para facilitar a leitura e a análise.
Cada entrada tem este formato:

```
data_hora|valor_original|base_origem|base_destino|resultado
```

Exemplo:

```
data_hora|valor_original|base_origem|base_destino|resultado
2026-05-19 22:15|255|10|16|FF
```

As colunas significam:
- `data_hora`: data e hora da conversão
- `valor_original`: número digitado pelo usuário
- `base_origem`: base do número original (2, 8, 10 ou 16)
- `base_destino`: base do resultado (2, 8, 10 ou 16)
- `resultado`: número convertido na base de destino

## Observações

- O programa valida entradas de binário e hexadecimal.
- Conversões de decimal para binário e decimal para hexadecimal são feitas com funções recursivas.
- O histórico não apaga entradas antigas; novas conversões são adicionadas no final do arquivo.

## Como testar

- Para ver se a conversão está correta, teste valores conhecidos: 10 decimal vira 1010 em binário e 255 decimal vira FF em hexadecimal.
- Para abrir o histórico, use a opção 6 e veja as linhas gravadas no arquivo.

Bom uso!