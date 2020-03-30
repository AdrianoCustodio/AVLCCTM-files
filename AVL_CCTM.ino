//Programa : AVL Temperature monitoring
//Alteracoes e adaptacoes : Adriano Custódio
//
//Baseado no programa exemplo de
//David A. Mellis, Tom Igoe e FILIPEFLOP

#include <SPI.h>
#include <Ethernet.h>
#include <mcp_can.h>
#include <SD.h>

// Tamanho do buffer utilizado para capturar os pedidos HTTP
#define REQ_BUF_SZ 80

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
float Temperatura[5][4], calculo,T_camara, T_agua1, T_agua2, T_oleo1, T_oleo2;
String acao;
int seleciona = 0;
int rele1 = 6;
int rele2 = 7;
int rele3 = 3;
boolean pagina = 1;
char HTTP_req[REQ_BUF_SZ] = {0};
char req_index = 0; // Índice no HTTP_req buffer 
File webFile;


MCP_CAN CAN0(8);                         // Seta CS para o pin 8
bool controle_1 =1, controle_2 =1, controle_3 =1, controle_4 =1, controle_5 =1, Camara1 = 0, Camara2 = 0, Camara3 = 0;


//Definicoes de IP, mascara de rede e gateway
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(169,254,208,200);          //Define o endereco IP
IPAddress gateway(169,254,208,200);     //Define o gateway
IPAddress subnet(255, 255, 255, 0);     //Define a máscara de rede
 
  //Inicializa o servidor web na porta 1000
  EthernetServer server(1000);
 
void setup()
{
        Serial.begin(19200);             // Configura a velocidade serial do Arduino
        Serial.println("OK");            // Mensagem OK informando que setou o baud rate para 19200
        CAN0.begin(CAN_1000KBPS);        // FOR THIS CONFIGURATION CONSIDER THE BOUND RATE DESIRED DIVIDED BY TWO!!!!!!!!. FOR EXAMPLE, YOU WISH 500 KBPS, THEN THE CONFIGURATION SHOULD BE 1000 KBPS!!!!  
        pinMode(2, INPUT);               // Seta pin 2 para /INT input
        Serial.println("MCP2515 initialazed");

// Desativar o chip de Ethernet
pinMode(10, OUTPUT);
digitalWrite(10, HIGH);

// Inicializa o SD card
Serial.println("Initializing SD card...");
if (!SD.begin(4)) {
Serial.println("ERROR - SD card initialization failed!");
return; // Inicialização falhou
}
Serial.println("SUCCESS - SD card initialized.");
// Verifica se exite o arquivo index.htm
if (!SD.exists("web.htm")) {
Serial.println("ERROR - Can't find web.htm file!");
return; // Não pode encontrar o arquivo especificado
}
Serial.println("SUCCESS - Found web.htm file.");
// Arquivo index.htm encontrado com sucesso
        
  //Inicializa a interface de rede
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2, HIGH);
  digitalWrite(rele3, HIGH);
}
 
void loop() 
{
 
if (!digitalRead(2))                     //Se o pin 2 esta em nível baixo, leia o buffer recebido
              {
                CAN0.readMsgBuf(&len, rxBuf);
                rxId = CAN0.getCanId();
                                
                if( rxId == 511 && controle_1)
                  {
                    Serial.print("ID: ");
                    Serial.print(rxId, HEX);
                    Serial.print(" ");
                    for(int i = 0; i<len; i++)                // Print each byte of the data
                      {
                        Serial.print(rxBuf[i], HEX);
                        Serial.print(" ");
                      }
                        Serial.println();
                       for (int i=0; i<len; i=i+2)
                         {                   
                          calculo = ((rxBuf[i])|(rxBuf[i+1]<<8));
                          Temperatura [0][(i*1)/2] = ((calculo*0.0625) - 273);              
                          }
                          for (int i=0; i<4; i++)
                            {
                              if (Temperatura[0][i] < -30)
                              {
                                Temperatura[0][i] = 0;
                              }
                              Serial.print(Temperatura[0][i]);
                              Serial.print(" ");
                            }                       
                   Serial.println();
                   controle_1 = 0; 
                   controle_5 = 1;
                  }

                   if( rxId == 767  && controle_2)
                  {
                    Serial.print("ID: ");
                    Serial.print(rxId, HEX);
                    Serial.print(" ");
                    for(int i = 0; i<len; i++)                // Print each byte of the data
                      {
                        Serial.print(rxBuf[i], HEX);
                        Serial.print(" ");
                      }
                      Serial.println();
                       for (int i=0; i<len; i=i+2)
                         {                   
                          calculo = ((rxBuf[i])|(rxBuf[i+1]<<8));
                          Temperatura [1][(i*1)/2] = ((calculo*0.0625) - 273);              
                          }
                          for (int i=0; i<4; i++)
                            {
                              if (Temperatura[1][i] < -30)
                              {
                                Temperatura[1][i] = 0;
                              }
                              Serial.print(Temperatura[1][i]);
                              Serial.print(" ");
                            }                        
                   Serial.println();
                   controle_2 = 0; 
                  }

                  if( rxId == 1023  && controle_3)
                  {
                    Serial.print("ID: ");
                    Serial.print(rxId, HEX);
                    Serial.print(" ");
                    for(int i = 0; i<len; i++)                // Print each byte of the data
                      {
                        Serial.print(rxBuf[i], HEX);
                        Serial.print(" ");
                      } 
                      Serial.println();
                       for (int i=0; i<len; i=i+2)
                         {                   
                          calculo = ((rxBuf[i])|(rxBuf[i+1]<<8));
                          Temperatura [2][(i*1)/2] = ((calculo*0.0625) - 273);              
                          }
                          for (int i=0; i<4; i++)
                            {
                              if (Temperatura[2][i] < -30)
                              {
                                Temperatura[2][i] = 0;
                              }
                              Serial.print(Temperatura[2][i]);
                              Serial.print(" ");
                            }                       
                   Serial.println();
                   controle_3 = 0; 
                  }
                  if( rxId == 1279  && controle_4)
                  {
                    Serial.print("ID: ");
                    Serial.print(rxId, HEX);
                    Serial.print(" ");
                    for(int i = 0; i<len; i++)                // Print each byte of the data
                      {
                        Serial.print(rxBuf[i], HEX);
                        Serial.print(" ");
                      }
                      Serial.println();
                       for (int i=0; i<len; i=i+2)
                         {                   
                          calculo = ((rxBuf[i])|(rxBuf[i+1]<<8));
                          Temperatura [3][(i*1)/2] = ((calculo*0.0625) - 273);              
                          }
                          for (int i=0; i<4; i++)
                            {
                              if (Temperatura[3][i] < -30)
                              {
                                Temperatura[3][i] = 0;
                              }
                              Serial.print(Temperatura[3][i]);
                              Serial.print(" ");
                            }                        
                   Serial.println();
                   controle_4 = 0; 
                  }

                  if( rxId == 385  && controle_5)
                  {
                    Serial.print("ID: ");
                    Serial.print(rxId, HEX);
                    Serial.print(" ");
                    for(int i = 0; i<len; i++)                // Print each byte of the data
                      {
                        Serial.print(rxBuf[i], HEX);
                        Serial.print(" ");
                      }
                      Serial.println();
                       for (int i=0; i<len; i=i+2)
                         {                   
                          calculo = ((rxBuf[i])|(rxBuf[i+1]<<8));
                          Temperatura [4][(i*1)/2] = ((calculo*0.0625) - 273);              
                          }
                          for (int i=0; i<4; i++)
                            {
                              if (Temperatura[4][i] < -30)
                              {
                                Temperatura[4][i] = 0;
                              }
                              Serial.print(Temperatura[4][i]);
                              Serial.print(" ");
                            }                        
                   Serial.println();
                   controle_1 = 1;
                   controle_2 = 1;
                   controle_3 = 1;
                   controle_4 = 1;
                   controle_5 = 0; 
                  }
              }          
                
   //Aguarda conexao do browser
  EthernetClient client = server.available();
  if (client)
    {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String linha = "";
    while (client.connected()) 
      {
      if (client.available())
        {
        char c = client.read(); 
        linha.concat(c);
        Serial.write(c);
           
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c; // Salva o atributo HTTP requerido
          req_index++;
          }  
          
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
                  
         
        if (c == 'n' && currentLineIsBlank)
          {        
         if (StrContains(HTTP_req, "ajax_inputs"))
           {                          
      // send a standard http response header
          //client.println("HTTP/1.1 200 OK");
         /* client.println("Content-Type: text/javascript");
          client.println("Connection: keep-alive");
          //client.println("Access-Control-Allow-Origin: *");
          client.println();*/
            
           int iniciofrente = linha.indexOf("?");
                //Serial.println(iniciofrente);
          if(iniciofrente>-1)//verifica se o comando veio
            {     
            iniciofrente     = iniciofrente+6; //pega o caractere seguinte
            int fimfrente    = iniciofrente+3; //esse comando espero 3 caracteres
            acao    = linha.substring(iniciofrente,fimfrente);//recupero o valor do comando
              if (acao == "001"||acao == "002"||acao == "003")
                  seleciona = 0;
            }
 
            if     ( acao == "001"||seleciona == 1)
              { 
                 T_camara = Temperatura[0][0];
                 T_agua1 = Temperatura[0][1];
                 T_oleo1 = Temperatura[0][2];
                 T_agua2 = Temperatura[0][3];
                 T_oleo2 = Temperatura[1][0];
                 Camara1 = 1;
                 Camara2 = 0;
                 Camara3 = 0;
                 seleciona = 1;
              } 
            
            else if ( acao == "002"||seleciona == 2)
              { 
                 T_camara = Temperatura[1][1];
                 T_agua1 = Temperatura[1][2];
                 T_oleo1 = Temperatura[1][3];
                 T_agua2 = Temperatura[2][0];
                 T_oleo2 = Temperatura[2][1];
                 Camara1 = 0;
                 Camara2 = 1;
                 Camara3 = 0;
                 seleciona = 2;
               } 
            
            else if ( acao == "003"||seleciona == 3)
              { 
                 T_camara = Temperatura[2][2];
                 T_agua1 = Temperatura[2][3];
                 T_oleo1 = Temperatura[3][0];
                 T_agua2 = Temperatura[3][1];
                 T_oleo2 = Temperatura[3][2];
                 Camara1 = 0;
                 Camara2 = 0;
                 Camara3 = 1;
                 seleciona = 3;
              }

            if ( acao == "004"){ digitalWrite(rele1, LOW); } 
            if ( acao == "005"){ digitalWrite(rele1, HIGH);} 
            
            if ( acao == "006"){ digitalWrite(rele2, LOW); }
            if ( acao == "007"){ digitalWrite(rele2, HIGH); }

            if ( acao == "008"){ digitalWrite(rele3, LOW); }
            if ( acao == "009"){ digitalWrite(rele3, HIGH); }
              } 
            else{
                        
          // Envia página web
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: keep-alive");
          // Abre o arquivo da página web
             webFile = SD.open("web.htm");
             Serial.println(webFile);
          if (webFile) {            
             while(webFile.available()) {
             client.write(webFile.read()); // Cliente recebe a página web
          }
             webFile.close();
          } 
          }              
            client.print("dados({ T_camara : ");
            client.print(T_camara);
            client.print(", T_agua1 :  ");
            client.print(T_agua1);
            client.print(", T_oleo1 : ");
            client.print(T_oleo1);
            client.print(", T_agua2 :  ");
            client.print(T_agua2);
            client.print(", T_oleo2 : ");
            client.print(T_oleo2);
            client.print(", Camara1 : ");
            client.print(Camara1);
            client.print(", Camara2 : ");
            client.print(Camara2);
            client.print(", Camara3 : ");
            client.print(Camara3);
            client.print(", rele1 : ");
            client.print(digitalRead(rele1));
            client.print(", rele2 : ");
            client.print(digitalRead(rele2));
            client.print(", rele3 : ");
            client.print(digitalRead(rele3));
            client.print("})");
           
          // Reseta o buffer para 0
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
         }
        if (c == 'n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != 'r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    }
}

// Seta todos os elementos de str para 0 (limpa o vetor)
void StrClear(char *str, char length)
{
for (int i = 0; i < length; i++) {
str[i] = 0;
}
}

char StrContains(char *str, char *sfind)
{
char found = 0;
char index = 0;
char len;

len = strlen(str);

if (strlen(sfind) > len) {
return 0;
}
while (index < len) {
if (str[index] == sfind[found]) {
found++;
if (strlen(sfind) == found) {
return 1;
}
}
else {
found = 0;
}
index++;
}

return 0;
}
