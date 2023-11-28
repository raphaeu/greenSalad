#include <ESP8266TimeAlarms.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define SENSOR_NIVEL_DAGUA_MIN  14
#define SENSOR_NIVEL_DAGUA_MAXIMO 12

#define RELE_BOMBA_DAGUA  10
#define RELE_VALVULA_DAGUA  16

#define LED_ON  0

#define WIFI_SSID "casa_dos_rafas"
#define WIFI_SENHA "felicidades"

#define TZ  PSTR("<-03>3")

struct tm *tm_struct;
bool flag_enchendo = 0;
bool flag_irrigando = 0;
bool flag_emergencia=0;
String ultimo_titulo = "";
String ultimo_texto = "";
String msgStatus="";
struct StructConfig {
  char horarios_funcionamento[96];
};
StructConfig MyConfig;



LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 20 chars and 4 line display
ESP8266WebServer server(80);

#include "Pages.h"

void setup() {
  Serial.begin(115200);

  lcd.init();                      // initialize the lcd   
  lcd.backlight();
  
  pinMode(RELE_BOMBA_DAGUA , OUTPUT);
  pinMode(RELE_VALVULA_DAGUA , OUTPUT);
  pinMode(LED_ON , OUTPUT);
  
  digitalWrite(RELE_BOMBA_DAGUA,LOW);
  digitalWrite(RELE_VALVULA_DAGUA,LOW);
  
  pinMode(SENSOR_NIVEL_DAGUA_MIN , INPUT_PULLUP);
  pinMode(SENSOR_NIVEL_DAGUA_MAXIMO , INPUT_PULLUP);

  EEPROM.begin(1024);
  EEPROM.get(0, MyConfig);
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);

  Serial.print("Conectando Wifi");
  displayInfo("Iniciando", "Conectando wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  

  displayInfo("Iniciando", "Sincronizando horario");
  Serial.println("Carregando informacoes de data e hora.");
  configTime(TZ, "pool.ntp.org");
  delay(500);

  Serial.print("IP: ");
  Serial.println(WiFi.localIP().toString());


  Alarm.timerRepeat(5, verificandoReservatorio);
  Alarm.timerRepeat(1, ligaLedON);
  Alarm.timerRepeat(2, desligaLedON);
  Alarm.timerRepeat(1, verificandoIrrigacao);

  server.on("/", HTTP_GET, handleIndex);
  server.on("/handleFormConfig", HTTP_GET, handleFormConfig);
  server.on("/save", HTTP_POST, handleSaveConfig);
  server.onNotFound(handleNotFound);
  server.begin();

  displayInfo("Informacao", "IP: "+WiFi.localIP().toString());

}

void loop() {
  server.handleClient();
  if (flag_enchendo == 0 && flag_irrigando == 0) {displayInfo("Informacao", "IP: "+WiFi.localIP().toString());}
  Alarm.delay(1); // wait one second between clock display
}

void ligaLedON(){
  digitalWrite(LED_ON, HIGH);
}
void desligaLedON(){
  digitalWrite(LED_ON, LOW);
}

void verificandoIrrigacao()
{
  int posicao = pegarPosicaoBaseadoHorario();

  Serial.print("Posicao:");Serial.print(posicao);
  Serial.print(" Valor:");Serial.println(MyConfig.horarios_funcionamento[posicao]);
  Serial.println(MyConfig.horarios_funcionamento);

  if (MyConfig.horarios_funcionamento[posicao] == '1') 
    ligaIrrigacao();
  else
    desligaIrrigacao();

}


void ligaIrrigacao()
{
  if (digitalRead(SENSOR_NIVEL_DAGUA_MIN) == 0)
  {
    if (flag_irrigando == 0)
    {
      displayInfo("Notificacao", "Irrigando a Horta.");
      digitalWrite(RELE_BOMBA_DAGUA,HIGH);
      flag_irrigando =1;
      Serial.println("Inicio da irrigacao");
    }
  }else{
    Serial.println("Nivel de agua abaixo do minimo.");
  }
}

void desligaIrrigacao()
{
  if (flag_irrigando == 1)
  {
    digitalWrite(RELE_BOMBA_DAGUA,LOW);
    flag_irrigando =0;
    Serial.println("Fim da irrigacao");
  }
}


void verificandoReservatorio(){
  
  if (flag_enchendo == 1){
    Serial.println("Enchendo reservatorio.");
  }
  else{
    Serial.print(" SENSOR_NIVEL_DAGUA_MIN:");Serial.print(digitalRead(SENSOR_NIVEL_DAGUA_MIN)?"Ativado":"ok");
    Serial.print(" SENSOR_NIVEL_DAGUA_MAXIMO:");Serial.println(digitalRead(SENSOR_NIVEL_DAGUA_MAXIMO)?"Ativado":"ok");
  }

  
  if (digitalRead(SENSOR_NIVEL_DAGUA_MIN) == 1 && digitalRead(SENSOR_NIVEL_DAGUA_MAXIMO) == 1 && flag_enchendo == 0 )encherDagua();
  if (digitalRead(SENSOR_NIVEL_DAGUA_MIN) == 0 && digitalRead(SENSOR_NIVEL_DAGUA_MAXIMO) == 0 && flag_enchendo == 1)pararEncherDagua(); 
}

void encherDagua(){
  if (flag_emergencia==0)
  {
    flag_enchendo = 1;
    displayInfo("Notificacao", "Enchendo reservatorio.");
    Alarm.timerOnce(5*60, pararEncherDaguaEmergencia); //forca desligaento em 5 minutos 
    Serial.println("Iniciando enchimento do reservatorio de agua.");
    digitalWrite(RELE_VALVULA_DAGUA, HIGH);
  }else{
    Serial.println("Bloqueado enchimento de agua, estado de emergencia.");
  }
}

void pararEncherDaguaEmergencia()
{
  flag_emergencia=1;
  Serial.println("Parada de emergencia.");
  pararEncherDagua();
}

void pararEncherDagua(){
  flag_enchendo = 0;
  Serial.println("Parando o enchimento do reservatorio de agua.");
  digitalWrite(RELE_VALVULA_DAGUA, LOW);
}


void displayInfo(String titulo, String texto){
  if(flag_emergencia) titulo = "! " + titulo + " !";
  if (titulo != ultimo_titulo || texto != ultimo_texto)
  {
    lcd.setCursor(0,0);
    lcd.print("                 ");
    lcd.setCursor(0,1);
    lcd.print("                 ");
  
    lcd.setCursor(17/2 - titulo.length()/2 ,0);
    lcd.print(titulo);  
    lcd.setCursor(17/2 - texto.length()/2 ,1);
    lcd.print(texto);
    ultimo_titulo = titulo;
    ultimo_texto = texto;  
  }
}

int pegarPosicaoBaseadoHorario()
{
    time_t now = time(NULL);
    tm_struct = localtime(&now);


    int x = 0;
    int h = tm_struct->tm_hour;
    int m = tm_struct->tm_min;

    Serial.print("horario:");Serial.print(h);Serial.print(':');Serial.println(m);

    if (m >= 0 ) x++;
    if (m > 15 ) x++;
    if (m > 30 ) x++;
    if (m > 45 ) x++;

    return (h * 4 + x)-1; 
}
