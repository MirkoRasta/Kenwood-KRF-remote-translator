# Kenwood KRF remote translator
Kenwood IR command translator for samsung remotes

L'Arduino Nano legge il segnale già presente sul pin OUT del sensore IR dell'amplificatore (non si
  interrompe nulla, si aggiunge solo un secondo ingresso in
  parallelo). Quando riconosce VOL+, VOL-, SRC+, SRC- o POWER
  del telecomando Samsung, genera:

    - una sequenza di quadratura sui pin A/B (volume o
      sorgenti) per simulare uno o più "scatti" della manopola
    - un impulso di chiusura sul pin POWER per simulare la
      pressione del tasto standby per accendere e spegnere l'amplificatore

 Il codice è stato scritto e testato su Arduino nano ma qualunque microcontrollore a 5V si può utilizzare direttamente senza traslatori di tensione.
 La funzionalità originale dell'amplificatore è stata mantenuta, è stata solo aggiunta la possibilità di comandarlo con i telecomandi delle tv samsung.
 I vari collegamenti sono stati effettuati secondo le seguenti immagini:
