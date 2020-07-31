//---------------------Blibiotecas-----------------------
#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Keypad.h>
//----------------------Definicões-------------------------
#define I2C_ADDRESS 0x3C//define o i2c para uso do oled
#define RST_PIN -1//define o pino de reset(oled)
#define PN532_SCK (14)//define o pino SCK
#define PN532_MOSI (16)//define pino mosi
#define PN532_SS (17)//define pino ss
#define PN532_MISO (15)//define pino miso
#define PN532_IRQ (18)//define pino irq
#define PN532_RESET (19)
#define NS=6//define o numero de senhas igual a 5
#define ledvermelho (11)
#define ledverde (12)
#define NS 6
//--------------------------variaveis globais----------------
char senha [NS][6] = {"12345", "54789", "55664", "88554", "64856", "75956"};//armazena as senhas que seram usadas
char possicao [6];//guarda a posição da tecla precionada
int contador = -1;// auxiliar de contagem 
char indx [NS][10] = {"Andre", "Pedro", "Jessica", "Larissa", "Poliana", "prof kenji"};//index de quem é cada senha
int s=0;
String hex_value = "";
bool cartaoreconhecido;
int u;
char resp;

const byte linhas = 4;
const byte colunas = 4;
char matriz_teclas [linhas][colunas] = 
{
 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};
byte pinoslinhas[linhas] = {9, 8, 7, 6};
byte pinoscolunas[colunas] = {5, 4, 3, 2};
//--------------------------explicação-----------------------
Keypad teclado = Keypad( makeKeymap(matriz_teclas), pinoslinhas, pinoscolunas, linhas, colunas);//mostra ao progama quem é o teclado
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);//mostra ao progama quem é nfc
SSD1306AsciiWire oled;
Servo servo_Motor;
void setup(){
//------------------------leds--------------
pinMode(ledvermelho, OUTPUT);
pinMode(ledverde, OUTPUT);
//-------------------------oled----------------
Wire.begin();
Wire.setClock(400000L);
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
oled.setFont(Cooper19);
//-----------------------serial---------------
Serial.begin(9600);
//--------------------- leitor Rf--------------
nfc.begin();
nfc.SAMConfig();
Serial.println("olá meu consagrado");
Serial.println("aguardando RFcard");
//--------------servo motor--------------
servo_Motor.attach(10); //PINO DE CONTROLE DO SERVO MOTOR
setlocked(true); //ESTADO INICIAL DA FECHADURA (TRANCADA)
  Serial.println("atualmente está sendo digitado");
}
void setlocked(int locked){ 
if (locked){ 
    servo_Motor.write(0); //POSIÇÃO DO SERVO FICA FECHADURA TRANCADA
    digitalWrite(ledvermelho, HIGH);// LED VERMELHO ACENDE
  digitalWrite(ledverde, LOW);// LED VERDE APAGA
}
else{ 
    servo_Motor.write(82);// SERVO GIRA ATÉ FECHADURA DESTRANCADA
     digitalWrite(ledvermelho, LOW);// LED VERMELHO ACENDE
      digitalWrite(ledverde, HIGH);// LED VERDE APAGA
  }
 
}

uint8_t cartaok;
void leituraRf(){
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // ARMAZENA A ID
  uint8_t uidLength;                        // DIZ SE O CARTÃO É DE 4 OU 7 BYTES 
  cartaok = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);// define 
    if (cartaok) {   
   // Serial.println("cartão encntrado");    
   // Serial.print("  UID tamanho: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
   // Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    for (uint8_t j=0; j < uidLength; j++) 
    {
    //  Serial.print(" 0x");Serial.print(uid[j], HEX);//transformando a id em uma string para comparar    
      //Serial.print(" ");Serial.print(uid[i], HEX);       
      hex_value += (String)uid[j];
    }
   // Serial.println(", value="+hex_value);   
    }
  }
void comparacard(){
if (hex_value == "1695248121"){
 // Serial.println("Cartao mestre reconhecido");
  cartaoreconhecido = true;
  oled.clear();
  oled.print("cartao");
  oled.setCursor(0,4);
  oled.print("mestre");
  return cartaoreconhecido;
  }
else if (hex_value == "771447889"){
 //Serial.println("cartao de Andre");
 cartaoreconhecido = true;
 oled.clear();
 oled.print("cartao de");
 oled.setCursor(0,4);
 oled.print("Andre ");
 u = 0;
 return u;
 return cartaoreconhecido;
}
else
//Serial.println("cartão não reconhecido");
cartaoreconhecido = false;
oled.clear();
oled.print("cartao nao");
oled.setCursor(0,4);
oled.print("reconhecido");
delay(5000);
return cartaoreconhecido;

}

void comparar(){
   //setlocked(true);//porta fechada
    //possicao[5]= '\0' ;
   contador=-1;
   int i=0;
   //Serial.println("valor atual de NS:");
   //Serial.println(NS);
  // Serial.println(String("Buscando /")+String(possicao));
   //Serial.println(u);    
     if (strcmp (possicao,senha[u]) == 0){
        contador=u;
     }
   else if (hex_value == "1695248121"){
       for(i=0;i < NS; i++){
        if (strcmp (possicao,senha[i]) == 0){
        contador=i;
       }
   }
  // Serial.println("valor contador encontrado");
   //Serial.println(contador);
   if(contador>=0)
   {
   //  Serial.print("encontrou a senha de");
     //Serial.println(contador);
     //Serial.println(indx[contador]);    
   }
    else{
    oled.clear();
    oled.print("senha icorreta");
     //Serial.print("essa senha nao existe");
    // Serial.println(contador);
     s=0;
     delay(10000);
  }
  
 }
}
void loginprof(){
  char sim ='A';
  char nao ='B';
   oled.clear();
   oled.print("senha de");
   oled.setCursor(0,4);
   oled.print("professor");
   delay(2000);
   oled.clear();
   oled.print("deseja ativar");
   oled.setCursor(0,4);
   oled.print("modo aula ?");
   delay(2000); 
  
   oled.clear();
   oled.print("sim = A");
   oled.setCursor(0,4);
   oled.print("nao = B");
   do{
    resp=teclado.getKey();
   }while (resp==0);
  // Serial.print(resp);   
     if(strcmp(resp,sim)==0){
      //Serial.print("passou no if");  
        tempodeaula();   
          if(strcmp(resp,nao)==0){
            autorizado();
          }
        } 
}
void tempodeaula(){
  char tecla;//variavel que coleta oque é digitado
  int z=0;//variavel pra possição
  //Serial.print("chegou no tempodeaula");
  oled.clear();
   oled.print("digite o");
   oled.setCursor(0,4);
   oled.print("tempo da aula");
   delay(2000);
   oled.clear();
   oled.print("deacordo com");
   oled.setCursor(0,4);
   oled.print("a tabela");
  do{
  tecla=teclado.getKey();
  }while(tecla==0);
  if(tecla!=0){    
     z=(int)tecla;
  }
  if (z != 0){
    //Serial.println("valor de z");
    //Serial.print(z);
    oled.clear();
    oled.print("aberto por");
    oled.setCursor(0,4);
    oled.print(" ");
    oled.print(z);
    oled.print(" min");
    setlocked(false);
    delay(z*60000);  
  }
}
void autorizado(){
   setlocked(false);//porta aberta
   //Serial.print("autorizado");
   oled.clear();
   
   oled.print("bem vindo");
   oled.setCursor(0,4);
   oled.print(indx[contador]);
   delay(5000);
  }
void loop(){
  setlocked(true); 
  oled.clear();
  oled.print("Aguardando");
  oled.setCursor(0,4);
  oled.print("o cartao");
  leituraRf(); 
  if (cartaok) {
    comparacard();
     if (cartaoreconhecido == true){ 
    do{
      char key;   
      do{
        key = teclado.getKey();
      } while(key==0);
      possicao[s++]= key;
    }while(s<5);
    possicao[5]='\0'; 
    comparar();    
 // Serial.println("valor contador encontrado");
  // Serial.println(contador);
    if (contador==5){
     loginprof();          
      }
   if (contador>=0){
    autorizado();
    }
      else{
      // Serial.println("nao autorizado");
        oled.clear();
        oled.print("Nao");
        oled.setCursor(0,4);
        oled.print("Autorizado");
        delay(5000);
      }
    }     
   }
  s=0;
  hex_value="";
  }
    
    


 


