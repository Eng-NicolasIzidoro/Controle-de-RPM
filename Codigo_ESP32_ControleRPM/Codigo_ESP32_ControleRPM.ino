// ================= Pinos do ESP32 =================
const int pinoPot = 34;    
const int pinoSensor = 18; 
const int pinoENA = 19;    
const int pinoIN1 = 21;    
const int pinoIN2 = 22;    

const int frequenciaPWM = 5000;
const int resolucaoPWM = 8; 

// IMPORTANTE: Se colocou 1 fita refletiva na hélice, deixe 1. 
// Se o sensor estiver a ler todas as pás da ventoinha sem fita (ex: 7 pás), mude para 7.
const int furosPorVolta = 1; 

// ================= Variáveis de Sistema =================
volatile unsigned int contadorPulsos = 0;
volatile unsigned long ultimoTempoInterrupcao = 0; 

// Filtro Antirruído Extremo (em microssegundos)
// 5000us = 5ms. Ignora os reflexos fantasmas e permite ler até 12.000 RPM com 1 fita.
// NOTA: Se estiver a ler 7 pás em vez de 1 fita, reduza este valor para 1000.
const unsigned long tempoDebounce = 5000; 

unsigned int rpmAtual = 0;
unsigned int maxRpmRegistrado = 0; 
int setpointGrafico = 0; 

int porcentagemAtual = 0; 
int modoControle = 0; 

unsigned long tempoAnterior = 0;
const int intervaloMedicao = 1000; 
float leituraPotFiltrada = 0; 

// ================= Interrupção (Filtro Antirruído de Alta Precisão) =================
void IRAM_ATTR contarPulsos() {
  unsigned long tempoAtual = micros(); // Resolução absoluta em microssegundos
  if (tempoAtual - ultimoTempoInterrupcao > tempoDebounce) { 
    contadorPulsos++;
    ultimoTempoInterrupcao = tempoAtual;
  }
}

// ================= Lógica de Motor =================
void atualizarMotor(int porcentagem) {
  if (porcentagem <= 1) { 
    digitalWrite(pinoIN1, LOW);
    digitalWrite(pinoIN2, LOW);
    ledcWrite(pinoENA, 0);
  } 
  else { 
    digitalWrite(pinoIN1, HIGH);
    digitalWrite(pinoIN2, LOW);
    int pwm = map(porcentagem, 2, 100, 15, 255);
    ledcWrite(pinoENA, pwm);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinoIN1, OUTPUT);
  pinMode(pinoIN2, OUTPUT);
  digitalWrite(pinoIN1, LOW); 
  digitalWrite(pinoIN2, LOW);

  analogReadResolution(12);
  pinMode(pinoPot, INPUT);
  leituraPotFiltrada = analogRead(pinoPot);

  ledcAttach(pinoENA, frequenciaPWM, resolucaoPWM);
  ledcWrite(pinoENA, 0);

  pinMode(pinoSensor, INPUT_PULLUP);
  // O modo FALLING aciona quando o reflexo da fita termina, sendo mais estável
  attachInterrupt(digitalPinToInterrupt(pinoSensor), contarPulsos, FALLING);
  
  tempoAnterior = millis();
}

void loop() {
  unsigned long tempoAtual = millis();

  // === 1. RECEPÇÃO DE COMANDOS VIA SERIAL ===
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd.startsWith("M")) { 
      modoControle = cmd.substring(1).toInt();
    } 
    else if (cmd.startsWith("P")) { 
      porcentagemAtual = cmd.substring(1).toInt();
      if(porcentagemAtual > 100) porcentagemAtual = 100;
      if(porcentagemAtual < 0) porcentagemAtual = 0;
      if (modoControle == 1) atualizarMotor(porcentagemAtual);
    }
    else if (cmd.startsWith("S")) { 
      setpointGrafico = cmd.substring(1).toInt();
    }
  }

  // === 2. CONTROLO FÍSICO (POTENCIÓMETRO) ===
  if (modoControle == 0) {
    int leituraBruta = analogRead(pinoPot); 
    leituraPotFiltrada = (0.1 * leituraBruta) + (0.9 * leituraPotFiltrada);
    porcentagemAtual = map((int)leituraPotFiltrada, 0, 4095, 0, 100);
    atualizarMotor(porcentagemAtual);
  }

  // === 3. MEDIÇÃO E ENVIO DE DADOS ===
  unsigned long tempoDecorrido = tempoAtual - tempoAnterior;
  if (tempoDecorrido >= intervaloMedicao) {
    noInterrupts();
    unsigned int pulsosLidos = contadorPulsos;
    contadorPulsos = 0; 
    interrupts();
    
    rpmAtual = (pulsosLidos / (float)furosPorVolta) * (60000.0 / tempoDecorrido);
    
    if (rpmAtual > maxRpmRegistrado) {
      maxRpmRegistrado = rpmAtual;
    }
    
    String json = "{\"rpm\":" + String(rpmAtual) + 
                  ",\"maxRpm\":" + String(maxRpmRegistrado) + 
                  ",\"pot\":" + String(porcentagemAtual) + 
                  ",\"modo\":" + String(modoControle) + 
                  ",\"setpoint\":" + String(setpointGrafico) + "}";
                  
    Serial.println(json);
    
    tempoAnterior = tempoAtual;
  }
}