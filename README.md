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
 I vari collegamenti sono stati effettuati sui segni rossi secondo le seguenti immagini:
L'alimentazione e il segnale IR sono stati presi dal sensore IR a bordo (A801 a schematico) dove il pin 1 è GND, pin 2 il segnale IR e il pin lato display di R862 per i 5V che alimentano l'arduino (precedentemente erano stati presi dal sensore IR ma mi sono accorto dopo che ha una resistenza in serie da 47R che con la corrente richiesta dell'arduino faceva scendere troppo la tensione e a volte il tasto di power non funzionava):
<img width="687" height="778" alt="image" src="https://github.com/user-attachments/assets/05b7aff0-b4ae-41fb-b7fe-f6a8d3880044" />

<img width="1074" height="894" alt="image" src="https://github.com/user-attachments/assets/e5273cba-268d-4dd3-953d-445aba267b15" />


 
per quanto riguarda il volume i pin A e B possono essere saldati sul retro dell'encoder o, come nel mio caso, ai capi di R910 e R911 per un KRF-v6080.

<img width="783" height="648" alt="image" src="https://github.com/user-attachments/assets/6c13c3a2-4f9c-40b5-88d6-64cfc1756606" />
<img width="1137" height="850" alt="image" src="https://github.com/user-attachments/assets/ccbedede-a81b-4f6b-a1fe-aa3f199189fc" />
<img width="917" height="907" alt="image" src="https://github.com/user-attachments/assets/d75e17a5-02c1-4d77-81bf-87bba6ee3626" />



Per le sorgenti ho utilizzato direttamente i pin dell'encoder:

<img width="707" height="542" alt="image" src="https://github.com/user-attachments/assets/eb5f3b5c-c0e2-4cd0-958d-f4f2c2b01881" />
<img width="1181" height="861" alt="image" src="https://github.com/user-attachments/assets/3d31e7f5-ab1b-4802-be52-9a128948bb7f" />


Il comando di power invece è stato preso qui, sul tasto. In questo caso non si deve collegare il pin dell'arduino direttamente con un filo come negli altri due casi ma va messo un resistore da 1k in serie (valore critico, non va bene 10k o altro, deve essere 1k perché il microcontrollore dell'amplificatore lo legge in analogico insieme al tasto "BAND" che ha un altro resistore.

<img width="720" height="614" alt="image" src="https://github.com/user-attachments/assets/24dafa28-8838-4057-a0eb-fceec20f4825" />
<img width="715" height="683" alt="image" src="https://github.com/user-attachments/assets/81533139-94e2-4aff-8bb5-51f398c85fc9" />

E questa è una foto dell'Arduino "provvisorio" che ci rimarrà con buona probabilità per tutta la vita
<img width="1243" height="909" alt="image" src="https://github.com/user-attachments/assets/adab77e0-4ae4-4f1f-9ca6-7ec6246dfc2b" />

Buon divertimento

English version:

# Kenwood KRF remote translator
Kenwood IR command translator for samsung remotes

The Arduino Nano reads the signal already present on the OUT pin of the amplifier's IR sensor (nothing is interrupted, a second input is simply added in parallel). When it recognizes VOL+, VOL-, SRC+, SRC-, or POWER from the Samsung remote, it generates:

    - a quadrature sequence on the A/B pins (volume or sources) to simulate one or more "clicks" of the knob
    - a closing pulse on the POWER pin to simulate pressing the standby button to turn the amplifier on and off

The code was written and tested on an Arduino Nano, but any 5V microcontroller can be used directly without voltage level shifters.
The amplifier's original functionality has been preserved; the ability to control it with Samsung TV remotes has just been added.
The various connections were made on the red marks according to the following images:
The power supply and the IR signal were taken from the onboard IR sensor (A801 on the schematic) where pin 1 is GND, pin 2 is the IR signal, and the display-side pin of R862 for the 5V to power the Arduino (previously they were taken from the IR sensor, but I realized later that it has a 47R series resistor which, with the current drawn by the Arduino, caused the voltage to drop too much and sometimes the power button didn't work):
<img width="687" height="778" alt="image" src="https://github.com/user-attachments/assets/05b7aff0-b4ae-41fb-b7fe-f6a8d3880044" />

<img width="1074" height="894" alt="image" src="https://github.com/user-attachments/assets/e5273cba-268d-4dd3-953d-445aba267b15" />

 
As for the volume, pins A and B can be soldered to the back of the encoder or, as in my case, to the ends of R910 and R911 for a KRF-v6080.

<img width="783" height="648" alt="image" src="https://github.com/user-attachments/assets/6c13c3a2-4f9c-40b5-88d6-64cfc1756606" />
<img width="1137" height="850" alt="image" src="https://github.com/user-attachments/assets/ccbedede-a81b-4f6b-a1fe-aa3f199189fc" />
<img width="917" height="907" alt="image" src="https://github.com/user-attachments/assets/d75e17a5-02c1-4d77-81bf-87bba6ee3626" />



For the sources, I used the encoder pins directly:

<img width="707" height="542" alt="image" src="https://github.com/user-attachments/assets/eb5f3b5c-c0e2-4cd0-958d-f4f2c2b01881" />
<img width="1181" height="861" alt="image" src="https://github.com/user-attachments/assets/3d31e7f5-ab1b-4802-be52-9a128948bb7f" />


The power command, instead, was taken here, on the button. In this case, you must not connect the Arduino pin directly with a wire as in the other two cases; a 1k resistor must be placed in series (critical value, 10k or other values won't work, it must be 1k because the amplifier's microcontroller reads it as an analog input along with the "BAND" button, which has another resistor).

<img width="720" height="614" alt="image" src="https://github.com/user-attachments/assets/24dafa28-8838-4057-a0eb-fceec20f4825" />
<img width="715" height="683" alt="image" src="https://github.com/user-attachments/assets/81533139-94e2-4aff-8bb5-51f398c85fc9" />

And this is a picture of the "temporary" Arduino which will most likely stay there for life.
<img width="1243" height="909" alt="image" src="https://github.com/user-attachments/assets/adab77e0-4ae4-4f1f-9ca6-7ec6246dfc2b" />

Have fun!
