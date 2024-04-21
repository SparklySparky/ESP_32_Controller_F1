# ESP32_Controller_F1
# Un controller che controlla una mini auto di formula 1 tramite protocollo espnow con esp 32

## Funzionalità

- Controllo dell'acceleratore e dello sterzo utilizzando i canali ADC (Analog to Digital Converter).
- Accensione e spegnimento della lettura dell'accelleratore e sterzo con pulsante
- Impostazione e rimozione del limitatore con un altro pulsante.
- Comunicazione wireless con l'auto tramite il protocollo ESP-NOW.

## Come Usare

1. Clona la repository sul tuo computer.
2. Configura l'hardware del controller.
3. Modifica le configurazioni GPIO in `car_control_gpios.h` per corrispondere alla tua configurazione.
4. Compila e carica il codice sul tuo controller.
5. Assicurati che l'auto sia configurata per ricevere i dati di controllo tramite il protocollo ESP-NOW (https://github.com/Tizio1234/esp32_f1).
6. Accendi i controller e inizia a controllare la mini auto di Formula 1 senza fili!

## Licenza

Questo progetto è rilasciato sotto la [GNU General Public License v3.0](LICENSE).
