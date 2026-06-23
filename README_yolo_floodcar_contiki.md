# Código Contiki 3.0 para simulação da hipótese YOLO-FloodCar

Este arquivo foi adaptado a partir da estrutura do exemplo enviado pelo usuário, mantendo o modelo de aplicação baseado em `collect`, `PROCESS`, `etimer`, sensores do Tmote Sky e envio periódico de pacotes.

## Arquivo principal

- `yolo_floodcar_contiki.c`

## O que o código simula

- Nó `1.0` como **sink**.
- Demais nós como **veículos/roteadores**.
- Cada nó envia periodicamente um pacote com:
  - luminosidade total
  - luminosidade fotossintética
  - temperatura
  - umidade
  - bateria
  - nível de água simulado
  - flag de risco de alagamento

## Lógica simplificada de alagamento

O código cria uma variável `water_level` simulada a partir de umidade, luz e temperatura. Isso permite gerar um cenário coerente no Cooja mesmo sem um sensor físico real de lâmina d'água.

A flag `flood=1` é acionada quando:
- a umidade está alta e a luminosidade baixa, ou
- o nível de água estimado é alto e a temperatura está abaixo do limiar configurado.

## Como usar no Cooja

1. Crie um novo mote type do tipo **Sky mote**.
2. Use o arquivo `yolo_floodcar_contiki.c` como firmware.
3. Instancie vários nós.
4. Garanta que o nó de ID `1.0` exista para atuar como sink.
5. Rode a simulação e observe o **Mote output**.

## Exemplo de topologia

- 1 sink
- 3 nós intermediários
- 6 a 8 nós sensores/veículos

## Saída esperada

Os nós emissores imprimem linhas como:

```text
TX no=4 payload=node=4,type=vehicle,sol=...,light=...,temp=...,hum=...,bat=...,water=...,flood=1
```

O sink imprime linhas como:

```text
RX sink | origem=4 seq=2 hops=3 dist_media=81 payload=node=4,type=vehicle,...,flood=1
```

## Observações

- O código usa a pilha `collect` do Rime, compatível com exemplos clássicos do Contiki 3.0.
- A simulação é voltada para apresentação acadêmica da topologia e do fluxo de eventos, não para validação final da hipótese de pesquisa.
- Os limiares podem ser ajustados nas `#define` do início do arquivo.
