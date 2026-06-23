# 🌊 YOLO-FloodCar: Simulação de Rede Veicular Multi-hop no Cooja

Este projeto foi desenvolvido como parte dos requisitos práticos da disciplina **Redes Avançadas 3** do Centro de Informática da Universidade Federal de Pernambuco (**CIn-UFPE**). 

A aplicação implementa e avalia a hipótese **YOLO-FloodCar**, utilizando uma arquitetura de rede sem fio *multi-hop* (múltiplos saltos) em nós sensores móveis e fixos para detectar e propagar alertas de risco de alagamento em vias urbanas.

---

## 💻 Sobre o Projeto

O objetivo do sistema é monitorar condições ambientais urbanas utilizando veículos como nós sensores móveis e roteadores dinâmicos. A simulação recria cenários de mobilidade veicular realistas passando pelas ruas ao redor e dentro do ecossistema da UFPE.

### Como Funciona:
1. **Geração de Mobilidade (SUMO):** A malha viária (`mapa.net.xml`) e os fluxos de tráfego foram modelados no SUMO. O rastro gerado (`trace.xml`) foi convertido em coordenadas de mobilidade (`mobilidade_cooja.txt`) compatíveis com o simulador Cooja.
2. **Sensoriamento e Lógica de Alagamento:** Veículos em movimento simulam a leitura de sensores integrados (luminosidade, temperatura, umidade e nível de água simulado). Caso as condições atinjam o limiar crítico (`FLOOD_THRESHOLD`), um alerta de alagamento é gerado.
3. **Comunicação Multi-hop (Rime Stack):** Os dados e alertas são transmitidos via pacotes de *broadcast* usando a pilha de rede Rime do Contiki, trafegando por nós roteadores intermediários até alcançarem o nó central receptor (**Sink**).

---

## 🛠️ Tecnologias e Ferramentas Utilizadas

* **Sistema Operacional / Simulador:** Instant Contiki 3.0 & Cooja Simulator
* **Linguagem de Programação:** C (voltado a sistemas embarcados)
* **Pilha de Rede:** Rime Stack (`broadcast` dinâmico com controle de saltos)
* **Plataforma de Hardware Emulada:** Tmote Sky (Sky Mote)
* **Gerador de Mobilidade:** SUMO (*Simulation of Urban MObility* versão 0.19.0)

---

## 📐 Arquitetura e Papéis da Rede

O arquivo de firmware `yolo_floodcar_contiki.c` define uma topologia lógica baseada no ID do nó (`linkaddr_node_addr`):

* **Nó 1 (Sink):** Nó central fixo e receptor final. Monitora a rede e imprime as mensagens agregadas recebidas de todas as origens.
* **Nós 2 a 4 (Routers):** Nós roteadores estáticos ou de trânsito estratégico que encaminham os pacotes (`FORWARDED`) incrementando o contador de saltos (*hop count*).
* **Nós 5 ou superiores (Vehicles):** Veículos em movimento que atuam como sensores móveis gerando dados de telemetria periódicos (`MSG_TYPE_STATUS`) ou alertas críticos (`MSG_TYPE_ALERT`).

---

## 📂 Estrutura do Repositório

* `yolo_floodcar_contiki.c`: Código-fonte principal em C para o firmware dos nós no Contiki OS.
* `mobilidade_cooja.txt`: Arquivo de posições e movimentação dos nós traduzido para o plugin de mobilidade do Cooja.
* `mapa.net.xml`: Arquivo de rede viária gerado no SUMO (`netgenerate`).
* `trace.xml`: Exportação FCD (Floating Car Data) do SUMO contendo o passo a passo da simulação de tráfego original.

---

## 🔧 Como Executar a Simulação no Cooja

### Pré-requisitos
* Máquina virtual do **Instant Contiki 3.0** configurada.
* Plugin de mobilidade (`Mobility Plugin`) ativo no Cooja.

### Passo a Passo

1. **Clonar o Repositório:**
   ```bash
   [git clone [https://github.com/seu-usuario/nome-do-repositorio.git](https://github.com/seu-usuario/nome-do-repositorio.git)
   cd nome-do-repositorio](https://github.com/yulleguga-bit/Contiki-3.0---Food-Car.git)
