#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Sensores solo
int soloPin[4] = {A0, A1, A2, A3};

// Relés
int relePin[4] = {8, 9, 10, 11};

// Variáveis
int solo[4];
int limite[4] = {20,20,20,20};

bool manual[4] = {false,false,false,false};
bool releEstado[4] = {false,false,false,false};

float temp;
float hum;

void setup() {
  Serial.begin(9600);
  dht.begin();

  for(int i=0;i<4;i++){
    pinMode(relePin[i], OUTPUT);
    digitalWrite(relePin[i], HIGH); // desligado (LOW ativa)
  }
}

void loop() {

  // ===== LEITURA SOLO =====
  for(int i=0;i<4;i++){
    int leitura = analogRead(soloPin[i]);
    solo[i] = map(leitura,1023,0,0,100);
  }

  // ===== DHT =====
  temp = dht.readTemperature();
  hum  = dht.readHumidity();

  // ===== LÓGICA =====
  for(int i=0;i<4;i++){

    if(manual[i]){
      // modo manual → mantém estado atual
    }else{
      // automático
      if(solo[i] < limite[i]){
        releEstado[i] = true;
      }else{
        releEstado[i] = false;
      }
    }

    digitalWrite(relePin[i], releEstado[i] ? LOW : HIGH);
  }

  // ===== ENVIA DADOS =====
  Serial.print(solo[0]); Serial.print(",");
Serial.print(solo[1]); Serial.print(",");
Serial.print(solo[2]); Serial.print(",");
Serial.print(solo[3]); Serial.print(",");

Serial.print(temp); Serial.print(",");
Serial.print(hum); Serial.print(",");

// estados dos relés (válvulas)
Serial.print(releEstado[0]); Serial.print(",");
Serial.print(releEstado[1]); Serial.print(",");
Serial.print(releEstado[2]); Serial.print(",");
Serial.println(releEstado[3]);

  // ===== RECEBE COMANDOS =====
  if(Serial.available()){

    String cmd = Serial.readStringUntil('\n');

    // ===== MANUAL =====
    if(cmd.startsWith("M")){
      int id = cmd.substring(1).toInt() - 1;

      manual[id] = !manual[id];

      // 👉 ESSA LINHA RESOLVE SEU PROBLEMA
      releEstado[id] = !releEstado[id];
    }

    // ===== LIMITE =====
    if(cmd.startsWith("L")){
      int id = cmd.substring(1,2).toInt() - 1;
      int valor = cmd.substring(3).toInt();

      limite[id] = valor;
    }
  }

  delay(1500);
}