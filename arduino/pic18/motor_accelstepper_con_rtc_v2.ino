// ============================================================
//  MOTOR 28BYJ-48 + DRIVER ULN2003 + RTC DS1302
//  Arduino Nano | AccelStepper con medio paso (half step)
//
//  INSTALAR en Library Manager:
//    - "AccelStepper" de Mike McCauley
//    - "Rtc by Makuna"
//
//  PINES MOTOR:
//    IN1 → D8  |  IN2 → D9  |  IN3 → D10  |  IN4 → D11
//    VCC → 5V  |  GND → GND
//
//  PINES RTC:
//    DAT → D6  |  CLK → D5  |  RST → D7
//    VCC → 5V  |  GND → GND
//
//  PRIMERA VEZ: pon SINCRONIZAR_HORA en 1, sube, espera el
//  mensaje en el monitor serie, luego ponlo en 0 y vuelve a subir.
// ============================================================

#include <AccelStepper.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

// ── Pon en 1 solo la primera vez para sincronizar la hora ──
#define SINCRONIZAR_HORA  0
// ──────────────────────────────────────────────────────────

// ── Ajusta estos valores ──────────────────────────────────
const float    DISTANCIA_CM  = 3.2;    // cm que quieres mover por ciclo
const float    DIAMETRO_CM   = 20.41;  // diámetro de la polea en CM
const uint32_t INTERVALO_SEG = 5;      // cada cuánto mover en SEGUNDOS
// ─────────────────────────────────────────────────────────

const int PASOS_POR_VUELTA = 4096;  // con medio paso son 4096 (no 2048)

// Cálculo de pasos
const int PASOS = (DISTANCIA_CM * PASOS_POR_VUELTA) / (3.141592 * DIAMETRO_CM);

// RTC: DAT=D6, CLK=D5, RST=D7
ThreeWire myWire(6, 5, 7);
RtcDS1302<ThreeWire> rtc(myWire);

// HALF4WIRE = medio paso — mismo orden de pines que tu código
AccelStepper motor(AccelStepper::HALF4WIRE, 8, 9, 10, 11);
#define PIN_HALL   2
#define PIN_BUZZER 12

uint32_t ultimoGiro = 0;
bool moviendose = false;

void apagarMotor() {
  motor.disableOutputs();
}

void setup() {
  Serial.begin(9600);
  motor.setMaxSpeed(800);
  motor.setAcceleration(1000);
  motor.enableOutputs();

  rtc.Begin();

  if (!rtc.GetIsRunning()) {
    Serial.println("[RTC] Reloj detenido, arrancando...");
    rtc.SetIsRunning(true);
  }

#if SINCRONIZAR_HORA == 1
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  rtc.SetDateTime(compiled);
  Serial.println("[RTC] Hora sincronizada con la computadora.");
  Serial.println("      >>> Ahora pon SINCRONIZAR_HORA en 0 y vuelve a subir. <<<");
#endif

  RtcDateTime ahora = rtc.GetDateTime();
  if (!ahora.IsValid()) {
    Serial.println("[ERROR] Fecha del RTC inválida.");
    Serial.println("        Pon SINCRONIZAR_HORA en 1, sube y luego ponlo en 0.");
    while (true);
  }

  ultimoGiro = ahora.Epoch32Time();

  Serial.println("=== Motor listo (con RTC, AccelStepper) ===");
  Serial.print("  Pasos por ciclo : "); Serial.println(PASOS);
  Serial.print("  Intervalo       : "); Serial.print(INTERVALO_SEG); Serial.println(" seg");

  if (PASOS == 0) {
    Serial.println("[ERROR] Pasos = 0. Revisa DISTANCIA_CM y DIAMETRO_CM.");
    while (true);
  }
}

void loop() {
  RtcDateTime ahora = rtc.GetDateTime();

  if (!ahora.IsValid()) {
    Serial.println("[ERROR] Lectura de RTC inválida, reintentando...");
    delay(1000);
    return;
  }

  uint32_t tiempoActual = ahora.Epoch32Time();

  // Si ya pasó el intervalo y el motor está quieto, iniciar movimiento
  if (!moviendose && (tiempoActual - ultimoGiro >= INTERVALO_SEG)) {
    Serial.print("[MOTOR] Moviendo "); Serial.print(PASOS); Serial.println(" pasos...");
    motor.enableOutputs();
    motor.move(PASOS);
    moviendose = true;
  }

  // Ejecutar el movimiento paso a paso (no bloquea el loop)
  if (moviendose) {
    motor.run();

    if (motor.distanceToGo() == 0) {
      moviendose = false;
      ultimoGiro = tiempoActual;
      Serial.println("[MOTOR] Listo.");
      apagarMotor();
    }
  }
}
