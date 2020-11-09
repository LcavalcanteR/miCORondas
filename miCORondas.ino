/*   miCORondas:
        Um bot que fica ouvindo o chat da twitch para controlar luzes RGB no gabinete do computador (que é um casemod de microondas).
  
     Autor: Lucas Cavalcante (@LcavalcanteR).
     Sinta-se livre para modificar o programa e utilizá-lo como quiser, desde que me avise sobre o seu projeto pois sou curioso e gosto de ver projeto das pessoas.
*/
#include <ESP8266WiFi.h> //chama biblioteca de controle de wifi
#include <IRCClient.h>   //chama biblioteca de comunicação de chat IRC

int rgb = 0;
int r = 0;
int g = 0;
int b = 0;

int pinR = 4;
int pinG = 14;
int pinB = 12;

//IRC e twitch:
#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667
#define TWITCH_BOT_NAME "Nome-Do-Canal"
const String twitchChannelName = "nome-do-chat-minusculo";
String ircChannel = "";
#define TWITCH_OAUTH_TOKEN "INSIRA-TOKEN-AQUI" //pra pegar o token oAuth: https://twitchapps.com/tmi/

//inicia Wifi:
char ssid[] = "NOME-DA-REDE";
char password[] = "SENHA-D-WIFI";
WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

void setup() {
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); //inicia uma desconexao para conectar novamente (dependendo da placa isso não é necessário, na minha era).
  delay(100);
  WiFi.begin(ssid, password); //inicia o wifi
  while (WiFi.status() != WL_CONNECTED) { //mantem tentando conectar no wifi
    delay(500);
  }
  ircChannel = "#" + twitchChannelName;
  client.setCallback(callback);   //inicia o modo CallBack do IRC modo Client
}

void arcoiro() { //modo gambiarra de fazer controle de fade RGB
  if (rgb == 1) {
    viracor(1023, 0, 0);
  }
  client.loop();
  if (rgb == 1) {
    viracor(0, 1023, 0);
  }
  client.loop();
  if (rgb == 1) {
    viracor(0, 0, 1023);
  }
  client.loop();
  if (rgb == 1) {
    viracor(1023, 1023, 0);
  }
  client.loop();
  if (rgb == 1) {
    viracor(1023, 0, 1023);
  }
  client.loop();
  if (rgb == 1) {
    viracor(0, 1023, 1023);
  }
  client.loop();
}

void viracor(int red, int green, int blue) {        //recebe a cor objetivo
  while ( r != red || g != green || b != blue ) {   //caso a cor atual não seja igual a objetivo ele faz algumas operações
    if ( r < red ) r += 1;                          //basicamente, caso o vermelho atual seja menor que a meta, ele adiciona.
    if ( r > red ) r -= 1;                          //e caso seja maior que a meta, ele diminui até chegar na meta.
    if ( g < green ) g += 1;                        //o mesmo se repete pro verde
    if ( g > green ) g -= 1;
    if ( b < blue ) b += 1;                         //e pro azul também.
    if ( b > blue ) b -= 1;
    pinta();
    delayMicroseconds(1500);
  }
}
void pinta() {                                  // escreve nas portas digitais no formato PWM a intensidade de cada uma das cores.
  analogWrite(pinR, r);
  analogWrite(pinG, g);
  analogWrite(pinB, b);
}
void vermelho() {
  rgb = 0;
  r = 1023;
  g = 0;
  b = 0;
  pinta();
}
void verde() {
  rgb = 0;
  r = 0;
  g = 1023;
  b = 0;
  pinta();
}
void azul() {
  rgb = 0;
  r = 0;
  g = 0;
  b = 1023;
  pinta();
}
void laranja() {
  rgb = 0;
  r = 1023;
  g = 400;
  b = 0;
  pinta();
}
void roxo() {
  rgb = 0;
  r = 1023;
  g = 0;
  b = 1023;
  pinta();
}
void amarelo() {
  rgb = 0;
  r = 1023;
  g = 1023;
  b = 0;
  pinta();
}
void branco() {
  rgb = 0;
  r = 1023;
  g = 1023;
  b = 1023;
  pinta();
}
void rgbc() {
  rgb = 1;
  r = 0;
  g = 0;
  b = 0;
  pinta();
}
void SUB() {    //quando alguém dá sub ele pisca
  rgb = 0;
  for (int i = 0; i < 50; i++) {
    r = 0;
    g = 0;
    b = 1023;
    pinta();
    delay(90);
    r = 1023;
    g = 0;
    b = 0;
    pinta();
    delay(90);
  }
}
void FOLLOW() { //quando alguém dá follow pisca também, mas diferente
  rgb = 0;
  for (int j = 0; j < 35; j++) {
    r = 0;
    g = 0;
    b = 0;
    pinta();
    delay(90);
    r = 0;
    g = 0;
    b = 1023;
    pinta();
    delay(90);
  }
}

void loop() {
  arcoiro();
  pinta();
  if (!client.connected()) {
    if (client.connect(TWITCH_BOT_NAME, "", TWITCH_OAUTH_TOKEN)) { //inicia a conexao com as credenciais informadas lá em cima
      client.sendRaw("JOIN " + ircChannel);
      sendTwitchMessage("MiCORondas iniciado!"); //escreve no chat que esta conectado e pronto, se conectado de fato.
    } else {
      delay(5000);
    }
    return;
  }
  client.loop();
}

void sendTwitchMessage(String message) {
  client.sendMessage(ircChannel, message);
}

String hexa = "#ffffff";  //faz a conversão do modo RGB Hexadecimal pra RGB em valores PWM.
void convertelouco() {
  long long number = strtoll(&hexa[1], NULL, 16);
  r = number >> 16;
  g = number >> 8 & 0xFF;
  b = number & 0xFF;
  r = map(r, 0, 255, 0, 1023);
  g = map(g, 0, 255, 0, 1023);
  b = map(b, 0, 255, 0, 1023);
}

void callback(IRCMessage ircMessage) { //le o chat e verifica se algum comando foi chamado
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    ircMessage.nick.toUpperCase();
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    boolean falha = 0;
    if (ircMessage.text.indexOf("!#") > -1) {
      if (ircMessage.text[0] == '!') {
        for (int i = 0; i < 7; i++) {
          hexa[i] = ircMessage.text[i + 1];
          if (ircMessage.text[i + 1] > 'f') {
            falha = 1;
          }
        }
        if (falha == 1) {
          sendTwitchMessage("Favor conferir codigo hexadecimal. Valor informado é inválido.");
          falha = 0;
        }
        else {
          convertelouco();
        }
      }
    }
    if (ircMessage.text.indexOf("!rgb") > -1) {
      rgbc();
    }
    if (ircMessage.text.indexOf("!vermelho") > -1) {
      vermelho();
    }
    if (ircMessage.text.indexOf("!verde") > -1) {
      verde();
    }
    if (ircMessage.text.indexOf("!azul") > -1) {
      azul();
    }
    if (ircMessage.text.indexOf("!roxo") > -1) {
      roxo();
    }
    if (ircMessage.text.indexOf("!amarelo") > -1) {
      amarelo();
    }
    if (ircMessage.text.indexOf("!laranja") > -1) {
      laranja();
    }
    if (ircMessage.text.indexOf("!branco") > -1) {
      branco();
    }
    if (ircMessage.text.indexOf(":)") > -1 && ircMessage.nick == "STREAMELEMENTS") { //os comandos follow e sub são chamados apenas pelo streamelements, pra não acontecer de qualquer um no chat chamar
      FOLLOW();
    }
    if (ircMessage.text.indexOf(":D") > -1 && ircMessage.nick == "STREAMELEMENTS") {
      SUB();
    }
  }
}
