// ============================================================
//  MOTOR 28BYJ-48 + DRIVER ULN2003 — SIN RTC
//  Arduino Nano | AccelStepper con medio paso (half step)
//
//  INSTALAR: "AccelStepper" de Mike McCauley en Library Manager
//
//  PINES:
//    IN1 → D8  |  IN2 → D10  |  IN3 → D9  |  IN4 → D11
//    VCC → 5V  |  GND → GND
// ============================================================

#include <AccelStepper.h>

// ── Ajusta estos valores ──────────────────────────────────
const float DISTANCIA_CM  = 3.2;    // cm que quieres mover por ciclo
const float DIAMETRO_CM   = 20.41;  // diámetro de la polea en CM
const long  INTERVALO_MS  = 5000;   // cada cuánto mover (ms). 5000 = 5 seg
// ─────────────────────────────────────────────────────────

const int PASOS_POR_VUELTA = 4096;  // con medio paso son 4096 (no 2048)

// Cálculo de pasos
const int PASOS = (DISTANCIA_CM * PASOS_POR_VUELTA) / (3.141592 * DIAMETRO_CM);

// HALF4WIRE = medio paso — pines: IN1, IN3, IN2, IN4
AccelStepper motor(AccelStepper::HALF4WIRE, 8, 9, 10, 11);

unsigned long ultimoMovimiento = 0;
bool moviendose = false;

void apagarMotor() {
  motor.disableOutputs();
}

void setup() {
  Serial.begin(9600);
  motor.setMaxSpeed(800);
  motor.setAcceleration(1000);
  motor.enableOutputs();

  Serial.println("=== Motor listo (sin RTC, AccelStepper) ===");
  Serial.print("  Pasos por ciclo : "); Serial.println(PASOS);
  Serial.print("  Intervalo       : "); Serial.print(INTERVALO_MS / 1000); Serial.println(" seg");

  if (PASOS == 0) {
    Serial.println("[ERROR] Pasos = 0. Revisa DISTANCIA_CM y DIAMETRO_CM.");
    while (true);
  }
}

void loop() {
  unsigned long ahora = millis();

  // Si ya pasó el intervalo y el motor está quieto, iniciar movimiento
  if (!moviendose && (ahora - ultimoMovimiento >= INTERVALO_MS)) {
    Serial.print("[MOTOR] Moviendo "); Serial.print(PASOS); Serial.println(" pasos...");
    motor.enableOutputs();
    motor.move(PASOS);
    moviendose = true;
  }

  // Ejecutar el movimiento paso a paso (no bloquea el loop)
  if (moviendose) {
    motor.run();

    // Cuando termina el movimiento
    if (motor.distanceToGo() == 0) {
      moviendose = false;
      ultimoMovimiento = millis();
      Serial.println("[MOTOR] Listo.");
      apagarMotor(); // apaga bobinas para no calentar
    }
  }
}
