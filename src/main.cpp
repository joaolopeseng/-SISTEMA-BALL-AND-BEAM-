#include <Arduino.h>

#include <Servo.h>

float lerDistancia(); 


// --- Hardware ---
const int pinoTrig = 5;
const int pinoEcho = 6;
const int pinoServo = 3;

// --- Ganhos PID (Para ajustar no PC) ---
float Kp = -0.5; // --- O Kp serve
float Ki = 0.001; 
float Kd = 0.3; 

// --- Variáveis de Cálculo ---
float setpoint = 14; // Centro da gangorra
float erroAnterior = 0, erroIntegral = 0;
unsigned long tempoAnterior; // long é para numeros longos
                             // unsigned a avriavel não aceita numero negativo

Servo meuServo; // obijeto

void setup() {
  Serial.begin(9600);
  meuServo.attach(pinoServo); // fala qual porta o motor esta ligado
  pinMode(pinoTrig, OUTPUT); // arduino manda sinal para o sensor
  pinMode(pinoEcho, INPUT); // sensor manda sinal para o arduino
  tempoAnterior = millis(); // cronometra o tempo
}

void loop() {
  // Cálculo de tempo real 
  unsigned long tempoAtual = millis();
  float dt = (tempoAtual - tempoAnterior) / 1000.0;
  if (dt == 0) dt = 0.01; // Evita divisão por zero

  // 1. Leitura do Sensor
  float posicao = lerDistancia();



  // 2. Cálculo do Erro
  float erro = setpoint - posicao;

  // 3. PID
  float P = Kp * erro;
  
  erroIntegral += erro * dt;
  float I = Ki * erroIntegral; // O Ki que pediste para manter
  
  float D = Kd * (erro - erroAnterior) / dt;

  float ajuste = P + I + D;

  // 4. Atuação (90 é o centro)
  int angulo = 80 + ajuste;
  angulo = constrain(angulo, 60, 100);
  meuServo.write(angulo); // para o motor mexer

  // 5. SAÍDA PARA O GRÁFICO (Serial Plotter)
  // Formato: Valor1 Valor2 Valor3
  Serial.print("centro= ");
  Serial.print(setpoint); 
  Serial.print(" ");
  Serial.print("posição= ");
  Serial.print(posicao);
  Serial.print(" ");
  Serial.print("angulo= ");
  Serial.println(angulo); // Dividido por 10 para caber na escala do gráfico

  // Atualização
  erroAnterior = erro;
  tempoAnterior = tempoAtual;
  delay(30);
}

float lerDistancia() {
  digitalWrite(pinoTrig, LOW);
  delayMicroseconds(200);
  digitalWrite(pinoTrig, HIGH);
  delayMicroseconds(1000);
  digitalWrite(pinoTrig, LOW);
  long duracao = pulseIn(pinoEcho, HIGH, 3000);
  float d = duracao * 0.034 / 2;
  return (d > 40 || d <= 0) ? setpoint : d; 
}