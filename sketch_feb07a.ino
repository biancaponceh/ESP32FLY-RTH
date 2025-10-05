/*
 * ESP32FLY - RTH SYSTEM 
 * Versión corregida para Arduino IDE
 */

#include <TinyGPSPlus.h>

// ==================== CONFIGURACIÓN ====================

// Pines ESP32
#define LED_STATUS_PIN 2
#define BUZZER_PIN 13
#define PARACHUTE_PIN 12

// Parámetros
#define ACTIVATION_DELAY 5000

// Comunicación
#define GPS_SERIAL Serial2

// ==================== ENUMERACIONES ====================

enum FlightMode {
  MODE_DORMANT,
  MODE_EJECTION_DETECTED, 
  MODE_RTH_NAVIGATE,
  MODE_RTH_DESCEND,
  MODE_LANDED
};

// ==================== VARIABLES GLOBALES ====================

FlightMode currentMode = MODE_DORMANT;
unsigned long ejectionDetectedTime = 0;
unsigned long flightStartTime = 0;
int simulationPhase = 0;

TinyGPSPlus gps;

// ==================== SETUP ====================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("🚀 ESP32FLY RTH - ARDUINO IDE");
  Serial.println("==============================");
  
  // Configurar pines
  pinMode(LED_STATUS_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT); 
  pinMode(PARACHUTE_PIN, OUTPUT);
  digitalWrite(PARACHUTE_PIN, LOW);
  
  // Inicializar GPS simulado
  GPS_SERIAL.begin(9600);
  
  flightStartTime = millis();
  
  Serial.println("✅ Sistema listo - Modo DORMANT");
  beep(1500, 200);
  delay(200);
  beep(2000, 200);
}

// ==================== LOOP PRINCIPAL ====================

void loop() {
  unsigned long now = millis();
  
  updateSimulation(now);
  
  switch (currentMode) {
    case MODE_DORMANT:
      handleDormantMode(now);
      break;
    case MODE_EJECTION_DETECTED:
      handleEjectionDetectedMode(now);
      break;
    case MODE_RTH_NAVIGATE:
      handleRTHNavigateMode();
      break;
    case MODE_RTH_DESCEND:
      handleRTHDescendMode();
      break;
    case MODE_LANDED:
      handleLandedMode();
      break;
  }
  
  // Telemetría cada 3 segundos
  static unsigned long lastTelemetry = 0;
  if (now - lastTelemetry >= 3000) {
    sendTelemetry();
    lastTelemetry = now;
  }
  
  delay(100);
}

// ==================== SIMULACIÓN ====================

void updateSimulation(unsigned long now) {
  static unsigned long lastUpdate = 0;
  if (now - lastUpdate < 1000) return;
  lastUpdate = now;
  
  switch(simulationPhase) {
    case 0: // Lanzamiento (0-3 segundos)
      if (now - flightStartTime < 3000) {
        Serial.println("🚀 Cohete ascendiendo...");
      } else {
        simulationPhase = 1;
        Serial.println("💥 EYECCIÓN DETECTADA!");
        currentMode = MODE_EJECTION_DETECTED;
        ejectionDetectedTime = now;
      }
      break;
      
    case 1: // Espera después de eyección
      // Solo esperar el delay
      break;
      
    case 2: // Navegación RTH
      Serial.println("🧭 Navegando hacia HOME...");
      break;
  }
}

// ==================== MÁQUINA DE ESTADOS ====================

void handleDormantMode(unsigned long now) {
  blinkLED(1000);
}

void handleEjectionDetectedMode(unsigned long now) {
  unsigned long elapsed = now - ejectionDetectedTime;
  
  if (elapsed >= ACTIVATION_DELAY) {
    Serial.println("✅ Activando RTH...");
    currentMode = MODE_RTH_NAVIGATE;
    simulationPhase = 2;
    beep(1800, 300);
  } else {
    Serial.print("⏰ Delay: ");
    Serial.print((ACTIVATION_DELAY - elapsed) / 1000);
    Serial.println("s");
  }
  
  blinkLED(300);
}

void handleRTHNavigateMode() {
  static unsigned long navigateStart = 0;
  if (navigateStart == 0) {
    navigateStart = millis();
  }
  
  // Simular 5 segundos de navegación
  if (millis() - navigateStart < 5000) {
    Serial.println("🧭 Navegando RTH...");
  } else {
    Serial.println("🎯 HOME alcanzado - Descendiendo");
    currentMode = MODE_RTH_DESCEND;
  }
  
  blinkLED(400);
}

void handleRTHDescendMode() {
  static unsigned long descendStart = 0;
  if (descendStart == 0) {
    descendStart = millis();
  }
  
  // Simular 3 segundos de descenso
  if (millis() - descendStart < 3000) {
    Serial.println("⬇️  Descendiendo...");
  } else {
    Serial.println("🛬 ATERRIZAJE EXITOSO!");
    currentMode = MODE_LANDED;
    beep(1000, 2000);
  }
  
  blinkLED(600);
}

void handleLandedMode() {
  digitalWrite(LED_STATUS_PIN, HIGH);
  
  static unsigned long lastBeep = 0;
  if (millis() - lastBeep > 5000) {
    beep(2500, 200);
    lastBeep = millis();
    Serial.println("🏁 Misión completada - Esperando recuperación");
  }
}

// ==================== FUNCIONES AUXILIARES ====================

void blinkLED(int period) {
  digitalWrite(LED_STATUS_PIN, (millis() % period) < (period / 2));
}

void beep(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
  ledcSetup(0, frequency, 8);
  ledcAttachPin(pin, 0);
  ledcWrite(0, 128);
  delay(duration);
  ledcWrite(0, 0);
}

void sendTelemetry() {
  Serial.print("📊 TELEMETRÍA -> Modo: ");
  switch(currentMode) {
    case MODE_DORMANT: Serial.print("DORMANT"); break;
    case MODE_EJECTION_DETECTED: Serial.print("EYECCIÓN"); break;
    case MODE_RTH_NAVIGATE: Serial.print("NAVEGACIÓN"); break;
    case MODE_RTH_DESCEND: Serial.print("DESCENSO"); break;
    case MODE_LANDED: Serial.print("ATERRIZAJE"); break;
  }
  Serial.print(" | Tiempo: ");
  Serial.print((millis() - flightStartTime) / 1000);
  Serial.println("s");
}