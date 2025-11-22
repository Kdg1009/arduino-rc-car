#include "StateManager.h"
#include "secret.h"

// ===== Global Variables =====
StateManager& state = StateManager::instance();

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait up to 3 seconds for Serial
  
  Serial.println("========================================");
  Serial.println("       RC Car Control System");
  Serial.println("========================================");
  Serial.println();

  // Initialize StateManager (this initializes all subsystems)
  Serial.println("Initializing system...");
  state.init(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.println();
  Serial.println("========================================");
  Serial.println("System Ready!");
  Serial.print("IP Address: ");
  Serial.println(state.getIPAddress());
  Serial.println("Open browser and navigate to the IP above");
  Serial.println("========================================");
  Serial.println();
}

void loop() {
  unsigned long now = millis();
  
  // Update all subsystems through StateManager
  state.update(now);
  
  // Optional: Add a small delay to prevent overwhelming the system
  // Remove this if you need maximum responsiveness
  delay(10);
}