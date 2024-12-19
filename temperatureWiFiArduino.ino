#include <Preferences.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "FreeSans12pt7b.h"
#include <SPI.h>
#include <Fetch.h>

// Configuración de pines para la pantalla ILI9341
#define TFT_CS 21
#define TFT_DC 1
#define TFT_RST 0

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Colores personalizados
#define BACKGROUND_COLOR ST77XX_BLACK
#define TEXT_COLOR ST77XX_WHITE
#define TEMP_COLOR ST77XX_RED
#define HUMIDITY_COLOR ST77XX_BLUE
#define WEATHER_COLOR ST77XX_GREEN
#define ST77XX_DARKGREY 0x4208
#define BOX_COLOR 0x4208

Preferences pref;
 
const char *SSID = "APlus Fibra-1513A4";
const char *PASSWORD = "qNiBLVsc";

StaticJsonDocument<4096> doc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tft.init(172,320);
  tft.setFont(&FreeSans12pt7b);
  tft.setRotation(1); // Ajusta según la orientación de tu pantalla

  // Limpiar pantalla
  tft.fillScreen(BACKGROUND_COLOR);

  while(!Serial);

  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  String temperatureJson = httpGetWeather();
  if(temperatureJson != ""){
    deserializeJson(doc, temperatureJson);
    Serial.print("JSON Data: ");
    String temp = (String)redondearDosDecimales(doc["current"]["temperature_2m"]);
    String humidity = (String)redondearDosDecimales(doc["current"]["relative_humidity_2m"]);
    Serial.print(temp);
    Serial.println(humidity);
    drawHeader();
    drawWeatherBoxes(temp, humidity, "Soleado");
    drawFooter();
  }

}

void loop() {
  String temperatureJson = httpGetWeather();
  Serial.println(temperatureJson);
  if(temperatureJson != ""){
    deserializeJson(doc, temperatureJson);
    Serial.print("JSON Data: ");
    String temp = (String)redondearDosDecimales(doc["current"]["temperature_2m"]);
    String humidity = (String)redondearDosDecimales(doc["current"]["relative_humidity_2m"]);
    Serial.print(temp);
    Serial.println(humidity);
    drawHeader();
    drawWeatherBoxes(temp, humidity, "Soleado");
    drawFooter();
  }
  delay(1800000);
}

String httpGetWeather(){
  String route = "https://api.open-meteo.com/v1/forecast?latitude=37.9923795&longitude=-1.1305431&current=temperature_2m,relative_humidity_2m,rain,weather_code";
  
  RequestOptions options;
  options.method = "GET";
  // options.fingerprint = "DC 78 3C 09 3A 78 E3 A0 BA A9 C5 4F 7A A0 87 6F 89 01 71 4C";
  options.caCert = "";
  options.headers["Content-Type"] = "application/json";
  Response response = fetch("https://api.open-meteo.com/v1/forecast?latitude=37.9923795&longitude=-1.1305431&current=temperature_2m,relative_humidity_2m,rain,weather_code", options);
  int startIndex = response.text().indexOf('{');          // Encontrar el índice del primer '{'

  if (startIndex != -1) { // Si se encuentra '{'
      String result = response.text().substring(startIndex); 
      return result;
  } else {
      Serial.println("No se encontró un diccionario en la cadena.");
      return "";
  }
}

void drawHeader() {
  tft.fillRect(0, 0, 320, 40, ST77XX_DARKGREY); // Barra superior
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(20, 30); // Posicionar texto manualmente
  tft.setTextSize(1);
  tft.print("Clima Actual"); // Título
}

void drawFooter() {
  tft.fillRect(0, 140, 320, 40, ST77XX_DARKGREY); // Barra superior
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(20, 165); // Posicionar texto manualmente
  tft.setTextSize(1);
  tft.print("Coded by 0c0de"); // Título
}

void drawWeatherBoxes(String temperature, String humidity, const char* weather) {
  // Limpiar la pantalla principal
  tft.fillRect(0, 40, 172, 200, BACKGROUND_COLOR);

  // Caja de temperatura
  tft.fillRect(10, 50, 140, 70, BOX_COLOR);
  tft.drawRect(10, 50, 140, 70, BOX_COLOR);
  tft.setTextColor(TEXT_COLOR);
  tft.setTextSize(1);
  tft.setCursor(30, 90);
  tft.print(String(temperature) + "°C");

  // Caja de humedad
  tft.fillRect(170, 50, 140, 70, BOX_COLOR);
  tft.drawRect(170, 50, 140, 70, BOX_COLOR);
  tft.setTextColor(TEXT_COLOR);
  tft.setTextSize(1);
  tft.setCursor(180, 90);
  tft.print(String(humidity) + "%");
}

double redondearDosDecimales(double valor) {
  return round(valor * 100.0) / 100.0;
}
