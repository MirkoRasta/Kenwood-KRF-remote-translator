/*
  ============================================================
  Bridge IR Samsung TV -> Controllo Amplificatore KRF-V6080
  ============================================================
  L'Arduino Nano legge in DERIVAZIONE il segnale già presente
  sul pin OUT del sensore IR dell'amplificatore (non si
  interrompe nulla, si aggiunge solo un secondo ingresso in
  parallelo). Quando riconosce VOL+, VOL-, SRC+, SRC- o POWER
  del telecomando Samsung, genera:

    - una sequenza di quadratura sui pin A/B (volume o
      sorgenti) per simulare uno o più "scatti" della manopola
    - un impulso di chiusura sul pin POWER per simulare la
      pressione del tasto standby

  USCITE OPEN-COLLECTOR:
  A riposo i pin sono in INPUT (alta impedenza, nessun pull-up
  interno attivato) e si affidano alle resistenze di pull-up
  già presenti sulla scheda dell'amplificatore per lo stato
  alto. Quando "premuti" vanno in OUTPUT e forzano LOW.
  Così facendo possono essere collegati in parallelo ai
  contatti originali degli encoder e del pulsante senza alcun
  conflitto, qualunque sia la tensione di pull-up lato ampli
  (l'Arduino non porta mai corrente attiva verso l'alto,
  tira solo verso massa).

  Libreria richiesta: IRremote (v4.x) dal Library Manager.
  ============================================================
*/

#include <IRremote.hpp>

// ---------------- PIN ----------------
#define IR_RECEIVE_PIN   2   // derivazione dall'uscita del sensore IR dell'ampli

#define PIN_ENC_A        5   // in parallelo al pin A dell'encoder volume
#define PIN_ENC_B        4   // in parallelo al pin B dell'encoder volume
#define PIN_SRC_A        8   // in parallelo al pin A dell'encoder sorgenti
#define PIN_SRC_B        7   // in parallelo al pin B dell'encoder sorgenti
#define PIN_POWER        6   // in parallelo al pulsante standby/power
#define PIN_LED          13  // LED integrato: lampeggia a ogni comando decodificato

// ---------------- CODICI IR SAMSUNG ----------------
// Valori confermati con lo sniffer (campi Address/Command già
// decodificati da IRremote, protocollo Samsung):
#define IR_ADDRESS       0x07
#define CMD_VOL_UP       0x07
#define CMD_VOL_DOWN     0x0B
#define CMD_POWER        0x02
#define CMD_SRC_UP       0x15
#define CMD_SRC_DOWN     0x16

// ---------------- PARAMETRI ENCODER SIMULATO ----------------
const uint8_t  STEPS_PER_PRESS  = 1;     // "scatti" per ogni pressione del tasto
const uint8_t  STEPS_PER_REPEAT = 1;     // scatti per ogni ripetizione (tasto tenuto premuto)
const uint16_t STEP_DELAY_US    = 2500;  // durata di ogni semifase del segnale di quadratura
                                          // -> aumenta se l'ampli non "sente" gli scatti,
                                          //    diminuisci se sale/scende troppo piano

const uint16_t POWER_PULSE_MS   = 150;   // durata pressione simulata del tasto power
                                          // (regola in base a quanto l'ampli richiede per
                                          //  riconoscere una pressione valida)

const uint16_t LED_BLINK_MS     = 60;    // durata del lampeggio del LED

const uint16_t COMMAND_COOLDOWN_MS = 300; // tempo minimo (ms) tra due comandi "nuovi"
                                           // QUALSIASI (non solo identici): scarta come
                                           // rimbalzo/doppio invio ogni frame che arriva
                                           // troppo a ridosso del precedente, anche se il
                                           // comando decodificato risultasse diverso (es.
                                           // per un bit corrotto sulla derivazione del
                                           // segnale). Aumenta se vedi ancora doppi,
                                           // diminuisci se ti sembra "lento" a registrare
                                           // pressioni ravvicinate volute.

const uint16_t MIN_ACTION_GAP_MS = 100;   // intervallo minimo (ms) tra due AZIONI
                                           // qualsiasi, comprese quelle generate dai
                                           // repeat: a differenza del cooldown sopra,
                                           // questo si applica anche mentre tieni premuto
                                           // un tasto, evitando che repeat troppo fitti
                                           // generino più scatti del dovuto. Aumenta se
                                           // vedi ancora pressioni ravvicinate "fantasma",
                                           // diminuisci se il volume/sorgenti rispondono
                                           // troppo lentamente tenendo premuto.

uint8_t lastCommand = 0;
unsigned long lastPressTime = 0;
unsigned long lastActionTime = 0;
unsigned long ledOffTime = 0;            // 0 = LED spento / nessun lampeggio in corso

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  releaseOutput(PIN_ENC_A);
  releaseOutput(PIN_ENC_B);
  releaseOutput(PIN_SRC_A);
  releaseOutput(PIN_SRC_B);
  releaseOutput(PIN_POWER);
}

void loop() {
  if (IrReceiver.decode()) {
    handleIR();
    IrReceiver.resume();
  }

  // spegne il LED allo scadere del tempo di lampeggio, senza usare delay()
  if (ledOffTime != 0 && millis() >= ledOffTime) {
    digitalWrite(PIN_LED, LOW);
    ledOffTime = 0;
  }
}

void handleIR() {
  auto &d = IrReceiver.decodedIRData;
  bool isRepeat = d.flags & IRDATA_FLAGS_IS_REPEAT;
  unsigned long now = millis();
  uint8_t command;

  if (!isRepeat) {
    if (d.address != IR_ADDRESS) return;  // ignora comandi di altri telecomandi/device

    if (now - lastPressTime < COMMAND_COOLDOWN_MS) {
      return;                              // troppo a ridosso del comando precedente: rimbalzo/doppio invio
    }

    command = d.command;
    lastPressTime = now;
    lastCommand = command;                // memorizza per gli eventuali repeat
  } else {
    command = lastCommand;                // un repeat IR non porta address/command: riusa l'ultimo
  }

  // Gate finale: qualunque azione (nuova o repeat) deve rispettare un
  // intervallo minimo dall'ultima eseguita, altrimenti viene scartata.
  if (now - lastActionTime < MIN_ACTION_GAP_MS) {
    return;
  }
  lastActionTime = now;

  switch (command) {
    case CMD_VOL_UP:
      blinkLed();
      doEncoderSteps(PIN_ENC_A, PIN_ENC_B, true, isRepeat ? STEPS_PER_REPEAT : STEPS_PER_PRESS);
      break;

    case CMD_VOL_DOWN:
      blinkLed();
      doEncoderSteps(PIN_ENC_A, PIN_ENC_B, false, isRepeat ? STEPS_PER_REPEAT : STEPS_PER_PRESS);
      break;

    case CMD_SRC_UP:
      blinkLed();
      doEncoderSteps(PIN_SRC_A, PIN_SRC_B, true, isRepeat ? STEPS_PER_REPEAT : STEPS_PER_PRESS);
      break;

    case CMD_SRC_DOWN:
      blinkLed();
      doEncoderSteps(PIN_SRC_A, PIN_SRC_B, false, isRepeat ? STEPS_PER_REPEAT : STEPS_PER_PRESS);
      break;

    case CMD_POWER:
      blinkLed();
      if (!isRepeat) pressPower();   // ignora i repeat per il power (un solo toggle)
      break;
  }
}

// ---------------- LED DI SEGNALAZIONE ----------------
// Accende il LED e ne programma lo spegnimento (gestito nel loop),
// così non blocca la generazione dei segnali verso l'ampli.
void blinkLed() {
  digitalWrite(PIN_LED, HIGH);
  ledOffTime = millis() + LED_BLINK_MS;
}

// ---------------- USCITE OPEN-COLLECTOR ----------------
void driveLow(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void releaseOutput(uint8_t pin) {
  pinMode(pin, INPUT);   // alta impedenza: lo stato alto arriva dal pull-up
                          // già presente sulla scheda dell'amplificatore
}

// ---------------- SIMULAZIONE ENCODER A QUADRATURA ----------------
// Funzione generica: pinA/pinB indicano quale coppia di pin pilotare,
// così la stessa funzione serve sia per il volume sia per le sorgenti
// (o per qualunque altro encoder tu voglia aggiungere in futuro).
// Sequenza Gray a 2 bit, partendo da riposo (1,1):
//   aumento:    (1,1) -> (0,1) -> (0,0) -> (1,0) -> (1,1)
//   diminuzione:(1,1) -> (1,0) -> (0,0) -> (0,1) -> (1,1)
// Ogni transizione cambia un solo bit alla volta, come un
// vero encoder meccanico in quadratura.
void doEncoderSteps(uint8_t pinA, uint8_t pinB, bool increase, uint8_t steps) {
  for (uint8_t i = 0; i < steps; i++) {
    if (increase) {
      driveLow(pinA);        delayMicroseconds(STEP_DELAY_US);
      driveLow(pinB);        delayMicroseconds(STEP_DELAY_US);
      releaseOutput(pinA);   delayMicroseconds(STEP_DELAY_US);
      releaseOutput(pinB);   delayMicroseconds(STEP_DELAY_US);
    } else {
      driveLow(pinB);        delayMicroseconds(STEP_DELAY_US);
      driveLow(pinA);        delayMicroseconds(STEP_DELAY_US);
      releaseOutput(pinB);   delayMicroseconds(STEP_DELAY_US);
      releaseOutput(pinA);   delayMicroseconds(STEP_DELAY_US);
    }
  }
}

// ---------------- SIMULAZIONE PRESSIONE TASTO POWER ----------------
void pressPower() {
  digitalWrite(PIN_POWER, HIGH);
  pinMode(PIN_POWER, OUTPUT);
  delay(POWER_PULSE_MS);
  releaseOutput(PIN_POWER);
  digitalWrite(PIN_POWER, LOW);
}
