## >>>> Sistema SCADA de Controlo Térmico e Rotação (RPM) com ESP32

> Sistema de automação e monitorização em tempo real para motores DC (ventiladores), com interface gráfica web conectada diretamente via USB (Web Serial API). 

Este projeto substitui a comunicação Wi-Fi tradicional por uma ligação local de altíssima performance, ideal para ambientes industriais e aplicações de bancada onde a latência e a estabilidade da rede são críticas.

---

## >>>> Principais Funcionalidades

* **Telemetria de Alta Precisão (Tacómetro Digital):** Cálculo exato de RPM com base no intervalo dinâmico de tempo (Delta T).
* **Filtro Antirruído por Hardware/Software (Debounce):** Utilização de interrupções de hardware com bloqueio temporal via `micros()` (resolução de microssegundos) para ignorar ruído ótico (efeito repique) e reflexos falsos no sensor infravermelho.
* **Filtro EMA (Exponential Moving Average):** Suavização matemática da leitura do potenciómetro analógico, garantindo uma transição de potência limpa e isenta de trepidações.
* **Interface SCADA Local (Web Serial API):** Painel HMI construído em HTML5, CSS Grid e JavaScript puro que comunica diretamente com a porta COM/USB do ESP32 através do navegador.
* **Modo de Controlo Duplo:** Alternância dinâmica entre atuação física (potenciómetro na bancada) e atuação via software (slider na interface web).
* **Data Logger Integrado:** Registo contínuo de RPM e Potência com *timestamp* no painel, além de um gráfico Canvas com redimensionamento automático de escala e linha de *Setpoint* interativa.

---

## >>>> Arquitetura e Componentes

### Hardware
* **Microcontrolador:** ESP32 (Clássico)
* **Driver de Potência:** Ponte H L298N
* **Sensor de Velocidade:** Módulo Sensor Ótico Infravermelho (Refletivo)
* **Atuador:** Motor DC 12V (Ventoinha)
* **Input Analógico:** Potenciómetro Linear (10kΩ)

### Software
* **Backend (Embedded):** C/C++ (Arduino Core para ESP32)
* **Frontend (HMI):** HTML, CSS nativo, JavaScript (Canvas API, Web Serial API)

---

## >>>> Esquema de Ligações (Pinagem)

| Componente | Pino no ESP32 | Função |
| :--- | :--- | :--- |
| **Potenciómetro (Sinal)** | `GPIO 34` | Leitura analógica da potência desejada (ADC) |
| **Sensor IR (D0)** | `GPIO 18` | Interrupção externa (contagem de pulsos) |
| **Ponte H (ENA)** | `GPIO 19` | Saída PWM (Controlo de velocidade linear) |
| **Ponte H (IN1)** | `GPIO 21` | Controlo de direção (Sentido de rotação) |
| **Ponte H (IN2)** | `GPIO 22` | Controlo de direção |

> **Nota Crítica de Hardware:** O GND do ESP32, da Ponte H e do Sensor IR devem estar obrigatoriamente interligados para estabilizar a referência lógica. A ventoinha requer apenas **uma única marcação refletiva** no eixo para a medição perfeita de RPM.

---

## >>>> Como Executar o Projeto

1. **Gravação do Firmware:**
   * Abra o código-fonte C++ (`.ino`) na Arduino IDE.
   * Selecione a placa ESP32 e a porta COM respetiva.
   * Clique em "Carregar" (Upload).
   * **MUITO IMPORTANTE:** Assim que a gravação terminar, feche a janela do *Monitor Serial* da Arduino IDE. A porta COM precisa de estar livre para o navegador.

2. **Inicialização do Painel SCADA:**
   * Guarde o código de interface fornecido num ficheiro chamado `painel.html`.
   * Abra o ficheiro `painel.html` utilizando um navegador compatível com a leitura de hardware (recomenda-se **Google Chrome**, **Microsoft Edge** ou **Opera**).
   * Na interface, clique no botão **🔌 Conectar USB**.
   * Selecione a porta serial do ESP32 na janela suspensa do navegador e clique em Conectar. O fluxo de telemetria será iniciado instantaneamente com latência zero.

---

## >>>> Autor

**Nicolas Izidoro da Silva**  
*Desenvolvido como projeto prático de Engenharia de Controlo e Automação, com foco na integração entre Microcontrolador, controle de sistemas continuos, instrumentação industrial e sistemas de supervisão.*
