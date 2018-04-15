#include <Wire.h>
#include <NXTShield.h>

const bool CADASTRO = true, DEBUG = true; //Ativa a função de cadastro de IDs, mude para false para desabilitar
const byte RFID = 10; //Pino RFID
const unsigned short Users = 50; //Limite de cadastros RFID
unsigned long data[Users]; //Vetor que receberá os IDs
const byte inductive = 12;
typedef unsigned long ulong; //Cria uma "referência" ao tipo unsigned long, chamada de ulong. Utilizado devido à preguiça de se escrever "unsigned long" o tempo todo
typedef unsigned short ushort; //Cria uma "referência" ao tipo unsigned short, chamada de ushort. Utilizado devido à preguiça de se escrever "unsigned short" o tempo todo
long tempo_atual = 0;
const ushort Espera = 15000; //Tempo em ms para espera do usuário para pressionar o botão de cadastro

ulong usuario_atual = 0; //Variavel que receberá a posição do vetor que o usuário se encontra

uint8_t EmPwmCmd[4] = {
  0x44, 0x22, 0xbb, 0x01
};

int ultrapwm = 9, ultratrig = 4;

const byte plastico = 0, metal = 1, vidro = 2;

void setup() {
  if (DEBUG) Serial.begin(115200);
  pinMode(RFID, INPUT);
  for (int x = 0; x < Users; x++) {
    data[x] = 0; //Zera todos os cadastros
    Serial.println(data[x]);//Debug
  }
  //data[]={ID_1,ID_2,ID_3,ID_4,...}; //Definição manual dos cadastros
  //Inicia a comunicação Serial na velocidade de 155.200 bps
  ultraSetup(9, 4);
}

void loop() {
  //unsigned int tempo_atual = 0; //Tempo para função de cadastro
  ulong leitura = 0; //Variável do tipo 'unsigned long' ou 'ulong' neste código
  Serial.println("Apresente seu cartao: ");//Debug
  while (leitura == 0) leitura = decoder(); //leitura do ID do RFID

  if (!registrado(leitura) && CADASTRO) { //se não for registrado, o programa reinicia
    //Inicio do Cadastro
    Serial.println("Para se cadastrar e poder receber um desconto pelas garrafas, apresente seu cartao novamente:");//Debug
    tempo_atual = millis();
    leitura = 0;
    while (leitura == 0) {
      leitura = decoder();
      if ((millis() - tempo_atual) > Espera) {
        Serial.println("Tempo de espera atingido!");//Debug
        delay(500);
        Serial.println("Reiniciando o programa...");//Debug
        delay(150);
        loop(); //Chama o loop novamente, mas com cadastro feito
      }
    }
    cadastro(leitura);
    //Fim do Cadastro
  }//Fim do If
  else if (registrado(leitura)) { //Caso seja registrado, continua o programa
    Serial.print("Distancia: "); //Debug
    Serial.println(ultraCheck(ultrapwm, ultratrig)); //Debug
    if (ultraCheck(ultrapwm, ultratrig) > 2 && ultraCheck(ultrapwm, ultratrig) < 100) {
      switch (indutivo()) {
        case true:
          abre_porta(metal, true);
          break;
        case false:
          abre_porta(plastico, true);
          break;
      }
    }
  }
  delay(50);
}

boolean registrado(unsigned long ID) { //Verifica se o usuário já é cadastrado
  for (int x = 0; x < Users; x++) if (ID == data[x]) return true;
  Serial.println("Nao registrado!");//Debug
  return false;
}

void cadastro(unsigned long Leitura) {
  for (int x = 0; x < Users; x++) {
    if (data[x] == 0) {
      data[x] = Leitura;
      Serial.println("Cadastrado"); //Debug
      break;
    }
  }
  Serial.println(" ");//Debug
  Serial.println("Cadastro realizado com sucesso! Por favor aguarde...");//Debug
  delay(300);
  Serial.println(" ");//Debug
  Serial.println(" ");//Debug
  loop();
}

unsigned long decoder() {
  byte x = 0;
  int time_aux = 0;
  ulong data = 0;
  while (!digitalRead(RFID)) {
    time_aux++;
    delayMicroseconds(10);
  }
  if ((time_aux > 400) && (time_aux <= 1500)) {


    for (x = 0; x < 28; x++) {
      while (digitalRead(RFID));
      delayMicroseconds(250);
      if (digitalRead(RFID)) {
        return 0;
      }
      delayMicroseconds(400);
      if (digitalRead(RFID)) {
        data <<= 1;
      }
      else {
        data |= 0x1;
        data <<= 1;
      }
      delayMicroseconds(500);
      if (!digitalRead(RFID)) {
        return 0;
      }
    }
    return data;
  }
  else return 0;
}

bool indutivo() {
  Serial.print("Inductive Sensor Reading: ");//Debug
  Serial.println(!digitalRead(inductive));//Debug
  if (!digitalRead(inductive)) return true;
  return false;
}

void abre_porta(byte material, bool mecanism) { //se "mecanism" for true, as instruções com mecanismo de um motor apenas para as duas portinholas serão executadas
  //do contrario, apenas abrirá uma portinhola e a porta principal

  if (mecanism)
    switch (material) {
      case plastico:
        Motor2.move(FORWARD, 200, 500, BRAKE);
        delay(500);
        Motor1.move(FORWARD, 200, 1000, BRAKE);
        delay(700);
        Motor2.move(BACKWARD, 200, 500, BRAKE);
        Motor1.move(BACKWARD, 200, 1000, BRAKE);
        break;

      case metal:
        Motor2.move(BACKWARD, 200, 500, BRAKE);
        delay(500);
        Motor1.move(FORWARD, 200, 1000, BRAKE);
        delay(700);
        Motor2.move(FORWARD, 200, 500, BRAKE);
        Motor1.move(BACKWARD, 200, 1000, BRAKE);
        break;
    }
  else
    switch (material) {
      case plastico:
        Motor1.move(FORWARD, 200, 500, BRAKE);
        delay(700);
        Motor1.move(BACKWARD, 200, 500, BRAKE);
        break;

      case metal:
        Motor2.move(FORWARD, 200, 500, BRAKE);
        delay(700);
        Motor1.move(FORWARD, 200, 1000, BRAKE);
        delay(700);
        Motor2.move(BACKWARD, 200, 500, BRAKE);
        Motor1.move(BACKWARD, 200, 1000, BRAKE);
        break;
    }
}

void ultraSetup(int ultraPWM, int ultraTrig) {
  pinMode(ultraTrig, OUTPUT);                           // A low pull on pin COMP/TRIG
  digitalWrite(ultraTrig, HIGH);                        // Set to HIGH
  pinMode(ultraPWM, INPUT);                             // Sending Enable PWM mode command
}
/*------------------------------------------*/
int ultraCheck(int ultrapwm, int ultratrig) {
  unsigned int distance = 0;
  digitalWrite(ultratrig, LOW);
  digitalWrite(ultratrig, HIGH);
  unsigned long medicao = pulseIn(ultrapwm, LOW);
  if (medicao < 50000 && medicao >= 2) distance = medicao / 50;
  else return 50000;
  return distance;
} //Checa Ultrassonico e, conforme a distancia lida, checa o sensor indutivo



