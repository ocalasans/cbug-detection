# Cbug Detection

**Cbug Detection** è un include flessibile per **San Andreas Multiplayer (SA-MP)** progettato per rilevare l'exploit **C-Bug**, una tecnica che consente ai giocatori di sparare più velocemente del previsto sfruttando le meccaniche del gioco. Questo **include** offre agli **sviluppatori di server** uno strumento affidabile per identificare l'uso del **C-Bug**, consentendo l'integrazione con sistemi come anti-cheat, monitoraggio del comportamento o regole di gioco personalizzate.

Nel contesto di **SA-MP**, il **C-Bug** è una pratica che implica azioni rapide, come accovacciarsi o cambiare arma, dopo aver sparato con determinate armi, riducendo il tempo di ricarica e conferendo un vantaggio sleale nei combattimenti. Questo **include** rileva queste azioni in modo preciso, risultando utile in server **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, minigiochi o qualsiasi scenario che richieda un controllo rigoroso delle meccaniche di gioco. La sua versatilità consente utilizzi oltre l'anti-cheat, come l'analisi del gameplay o la creazione di meccaniche uniche.

> [!NOTE]
> **Cbug Detection** non si limita a bloccare il **C-Bug**; può essere integrato in sistemi di monitoraggio, eventi specifici o minigiochi con regole personalizzate. **In sintesi, sono gli sviluppatori a decidere quale azione intraprendere quando un giocatore viene rilevato usando il C-Bug**, garantendo un'ampia flessibilità nell'applicazione della rilevazione in base al contesto del server.

## Lingue

- Português: [README](../../)
- Deutsch: [README](../Deutsch/README.md)
- English: [README](../English/README.md)
- Español: [README](../Espanol/README.md)
- Français: [README](../Francais/README.md)
- Polski: [README](../Polski/README.md)
- Русский: [README](../Русский/README.md)
- Svenska: [README](../Svenska/README.md)
- Türkçe: [README](../Turkce/README.md)

## Indice

- [Cbug Detection](#cbug-detection)
  - [Lingue](#lingue)
  - [Indice](#indice)
  - [Funzionalità](#funzionalità)
  - [Come Funziona](#come-funziona)
    - [Logica di Rilevazione](#logica-di-rilevazione)
    - [Eventi Monitorati](#eventi-monitorati)
    - [Configurazioni (Defines)](#configurazioni-defines)
  - [Struttura del Codice](#struttura-del-codice)
  - [Installazione e Configurazione](#installazione-e-configurazione)
    - [Installazione](#installazione)
    - [Configurazione](#configurazione)
  - [Come Utilizzare](#come-utilizzare)
    - [Attivazione per Giocatore](#attivazione-per-giocatore)
    - [Disattivazione per Giocatore](#disattivazione-per-giocatore)
    - [Verifica dello Stato](#verifica-dello-stato)
    - [Integrazione con Anti-Cheat](#integrazione-con-anti-cheat)
    - [Registrazione delle Rilevazioni](#registrazione-delle-rilevazioni)
    - [Esempio in Minigioco](#esempio-in-minigioco)
  - [Applicazioni Possibili](#applicazioni-possibili)
  - [Personalizzazione](#personalizzazione)
    - [Regolazione dei Defines](#regolazione-dei-defines)
    - [Personalizzazione del Callback](#personalizzazione-del-callback)
  - [Test e Validazione](#test-e-validazione)
  - [Limitazioni e Considerazioni](#limitazioni-e-considerazioni)
  - [Licenza](#licenza)
    - [Condizioni](#condizioni)

## Funzionalità

- **Rilevazione Completa del C-Bug**: Identifica le seguenti varianti:
  - Clássico
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Spari rapidi
- **Controllo per Giocatore**: Attiva o disattiva la rilevazione individualmente per ogni giocatore.
- **Adattamento al Ping**: Adatta i tempi di rilevazione in base al ping del giocatore, riducendo al minimo i falsi positivi.
- **Protezione contro Falsi Positivi**: Valida lo stato del giocatore, l'arma, le munizioni e le animazioni per garantire precisione.
- **Configurazioni Personalizzabili**: Definisce parametri di rilevazione tramite costanti (`defines`), sebbene i valori predefiniti siano ottimizzati.
- **Callback Estensibile**: Il callback `CbugDetection_OnDetected` consente l'integrazione con sistemi personalizzati.
- **Leggero e Ottimizzato**: Utilizza solo `OnPlayerKeyStateChange` e `OnPlayerWeaponShot`, con un impatto minimo sulle prestazioni.

## Come Funziona

L'include monitora le azioni dei giocatori attraverso i callback **OnPlayerKeyStateChange** e **OnPlayerWeaponShot**, utilizzando un sistema di **punteggio di sospetto** (`cbug_d_suspicion_score`) per identificare i pattern del C-Bug. Quando il punteggio supera una soglia configurabile (`CBUG_D_SUSPICION_THRESHOLD`), viene attivato il callback `CbugDetection_OnDetected`.

> [!IMPORTANT]
> Il sistema è progettato per essere robusto, con validazioni rigorose che minimizzano i **falsi positivi**, **sebbene casi rari possano verificarsi**, come dettagliato nella sezione delle limitazioni.

### Logica di Rilevazione
1. **Punteggio di Sospetto**:
   - Azioni sospette (es. accovacciarsi, cambiare arma, spari rapidi) incrementano `cbug_d_suspicion_score` con pesi specifici.
   - Esempio: Accovacciarsi dopo uno sparo aggiunge **4.0** punti; spari rapidi aggiungono **3.0** punti.
   - Il punteggio decade nel tempo (`CBUG_D_SUSPICION_DECAY = 0.5` al secondo) per evitare l'accumulo di azioni non correlate.

2. **Finestre Temporali**:
   - Le azioni sono considerate sospette se si verificano subito dopo uno sparo (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Gli spari rapidi vengono rilevati se si verificano entro `CBUG_D_SHOT_INTERVAL = 200ms`.

3. **Validazioni**:
   - Monitora solo armi specifiche (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - La rilevazione è disattivata se:
     - Il giocatore non è a piedi (`PLAYER_STATE_ONFOOT`).
     - Il giocatore sta correndo o saltando (verificato tramite animazioni).
     - Non ci sono munizioni (`GetPlayerAmmo(playerid) <= 0`).
     - La rilevazione è disattivata per il giocatore (`cbug_d_player_status`).

4. **Adattamento al Ping**:
   - La funzione `CbugDetection_GetPingAdjtThresh` regola i tempi in base al ping:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Questo garantisce equità per i giocatori con maggiore latenza (es. **200**-**300ms**).

5. **Cooldown**:
   - Un intervallo (`CBUG_D_COOLDOWN_TIME = 1500ms`) evita rilevazioni ripetute per la stessa sequenza.

### Eventi Monitorati
- **OnPlayerKeyStateChange**:
  - Traccia i tasti premuti (accovacciarsi, correre, sinistra/destra) e i cambi di arma.
  - Rileva pattern come accovacciarsi o rotolare dopo uno sparo.
  - Esempio:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Verifiche aggiuntive per Rollbug, Quick C-switch, ecc.
    }
    ```

- **OnPlayerWeaponShot**:
  - Monitora gli spari e le sequenze rapide.
  - Aggiorna il punteggio per spari consecutivi rapidi.
  - Esempio:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Inizializza lo stato di rilevazione del giocatore come disattivato e resetta le variabili:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Configurazioni (Defines)
I defines controllano il comportamento della rilevazione. I valori predefiniti sono ottimizzati e consigliati:

| Define                        | Valore Predefinito   | Descrizione                                                                            |
|-------------------------------|----------------------|----------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Punteggio necessario per la rilevazione. Valori più alti riducono la sensibilità.      |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Decadimento del punteggio al secondo.                                                  |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Finestra (**ms**) per considerare sospette le azioni dopo uno sparo.                   |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Finestra (**ms**) per rilevare spari rapidi consecutivi.                               |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Cooldown (**ms**) tra rilevazioni per evitare ripetizioni.                             |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Moltiplicatore per l'adattamento del ping nei tempi.                                   |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Buffer (**ms**) per la rilevazione del cambio arma dopo uno sparo.                     |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Tempo (**ms**) dopo l'ultima azione per resettare il punteggio.                        |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | Armi monitorate (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).   |

> [!WARNING]
> Non è consigliato modificare i valori dei defines, poiché i predefiniti sono stati calibrati con cura per bilanciare precisione e prestazioni.

## Struttura del Codice

L'include è organizzato in sezioni distinte, ognuna con una funzione specifica:

1. **Intestazione**:
   - Definisce il guard dell'include (`_cbug_detection_included`) e verifica l'inclusione di `a_samp` o `open.mp`.

2. **Configurazioni (Defines)**:
   - Definisce costanti per i parametri di rilevazione (es. `CBUG_D_SUSPICION_THRESHOLD`).
   - Usa `#if defined` per evitare conflitti di ridefinizione.

3. **Variabili Globali**:
   - Memorizza gli stati dei giocatori:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // Stato per giocatore
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Punteggio di sospetto
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Ultimo sparo
     // ... altre variabili per il tracciamento
     ```

4. **Funzioni Utilitarie (Static Stock)**:
   - Funzioni interne per la logica di rilevazione:
     - `CbugDetection_GetPingAdjtThresh`: Regola i tempi in base al ping.
     - `CbugDetection_IsCbugWeapon`: Verifica le armi monitorate.
     - `CbugDetection_IsJumping`: Rileva animazioni di salto.
     - `CbugDetection_IsRunning`: Rileva animazioni di corsa.
     - `CbugDetection_ResetVariables`: Resetta le variabili del giocatore.
   - Esempio:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... altre variabili da resettare...

         return true;
     }
     ```

5. **Funzioni Pubbliche (Stock)**:
   - Funzioni per il controllo del sistema:
     - `CbugDetection_Player`: Attiva o disattiva la rilevazione per un giocatore specifico.
     - `CbugDetection_IsActive`: Verifica se la rilevazione è attiva per un giocatore.
   - Esempio:
     ```pawn
     stock bool:CbugDetection_Player(playerid, bool:cbug_d_enable) {
         if (!IsPlayerConnected(playerid))
             return false;

         if (cbug_d_enable) {
             if (cbug_d_player_status[playerid])
                 return false;

             cbug_d_player_status[playerid] = true;

             return true;
         }
         else {
             if (!cbug_d_player_status[playerid])
                 return false;

             cbug_d_player_status[playerid] = false;
             CbugDetection_ResetVariables(playerid);

             return true;
         }
     }
     ```

6. **Callback**:
   - Implementa la logica in `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect`, e `OnPlayerDisconnect`.
   - Fornisce `CbugDetection_OnDetected` per la personalizzazione.
   - Esempio:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **Hook ALS**:
   - Integra i callback standard di SA-MP per la compatibilità:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... altri ALS da definire...
     ```

## Installazione e Configurazione

### Installazione
1. **Scarica l'Include**:
   - Posiziona il file `cbug-detection.inc` nella directory `pawno/include`.

2. **Includi nel Gamemode**:
   - Aggiungi all'inizio del gamemode:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> Assicurati di includere `a_samp` o `open.mp` prima di `cbug-detection.inc`, altrimenti il compilatore segnalerà errori.

3. **Compila il Gamemode**:
   - Usa il compilatore **Pawn** per compilare il gamemode, verificando l'assenza di errori.

### Configurazione
- **Definisci il Callback**:
  - Implementa `CbugDetection_OnDetected` nel gamemode per gestire le rilevazioni:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "Giocatore %d rilevato usando C-Bug!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Come Utilizzare

L'include offre un controllo individuale per la rilevazione del C-Bug per giocatore. Di seguito, esempi pratici di utilizzo:

### Attivazione per Giocatore
- Attiva per un giocatore specifico, ad esempio in un evento di combattimento:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Disattivazione per Giocatore
- Disattiva per un giocatore specifico, ad esempio per gli amministratori:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Verifica dello Stato
- Verifica se la rilevazione è attiva per un giocatore:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "La rilevazione è attiva per te.");
  ```

### Integrazione con Anti-Cheat
- Applica punizioni in `CbugDetection_OnDetected`:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Giocatore %s (ID: %d) rilevato usando C-Bug!", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Registrazione delle Rilevazioni
- Registra le rilevazioni in un file:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Giocatore %s (ID: %d) rilevato usando C-Bug\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Esempio in Minigioco
- Attiva la rilevazione solo per giocatori specifici in un minigioco:
  ```pawn
  CMD:iniciarminigioco(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "Rilevazione del C-Bug attivata per te nel minigioco!");

      return 1;
  }

  CMD:finalizarminigioco(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "Rilevazione del C-Bug disattivata per te!");

      return 1;
  }
  ```

> [!TIP]
> Usa `CbugDetection_IsActive` per verificare lo stato della rilevazione prima di eseguire azioni condizionali, come avvisi o punizioni.

## Applicazioni Possibili

**Cbug Detection** è altamente versatile e può essere utilizzato in diversi contesti:

- **Server Roleplay (RP)**: Rafforza le regole contro il **C-Bug** per combattimenti realistici.
- **Server TDM/DM**: Garantisce equilibrio nei combattimenti rilevando il **C-Bug**.
- **Monitoraggio**: Analizza il comportamento dei giocatori o identifica i trasgressori frequenti.
- **Minigiochi ed Eventi**: Controlla il **C-Bug** solo in eventi specifici.
- **Meccaniche Personalizzate**: Integra con sistemi di punteggio o penalità.

> [!NOTE]
> La capacità di preservare configurazioni individuali durante l'attivazione o disattivazione globale rende l'include ideale per server con regole dinamiche.

## Personalizzazione

Sebbene l'include sia ottimizzato con configurazioni predefinite, è possibile regolare i defines o il callback `CbugDetection_OnDetected`.

> [!WARNING]
> Modificare i defines o altre configurazioni **non è consigliato**, poiché i valori predefiniti sono stati testati e bilanciati per la massima efficacia. Le modifiche possono causare falsi positivi, rilevazioni imprecise o addirittura impedire il corretto funzionamento.

### Regolazione dei Defines
- **Server con Ping Alto**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Rilevazione più Rigorosa**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Armi Personalizzate**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Personalizzazione del Callback
- Aggiungi logica avanzata:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Avviso %d: Giocatore %s (ID: %d) rilevato usando C-Bug!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Uso ripetuto di C-Bug");
      
      return 1;
  }
  ```

## Test e Validazione

Per garantire il corretto funzionamento, esegui i seguenti test:

- **Rilevazione del C-Bug**:
  - Testa tutte le varianti elencate con le armi monitorate.
  - Conferma che `CbugDetection_OnDetected` viene attivato dopo **2**-**3** sequenze.

- **Falsi Positivi**:
  - Esegui azioni come correre, saltare o sparare senza accovacciarsi.
  - Verifica che non ci siano rilevazioni per azioni non correlate al **C-Bug**.

- **Controllo per Giocatore**:
  - Testa `CbugDetection_Player` per l'attivazione/disattivazione per giocatore.
  - Conferma che la rilevazione avviene solo per i giocatori con `cbug_d_player_status` attivo.

- **Verifica dello Stato**:
  - Usa `CbugDetection_IsActive` per confermare lo stato per giocatore.

- **OnPlayerConnect**:
  - Verifica che i nuovi giocatori inizino con la rilevazione disattivata (`cbug_d_player_status = false`).

- **Ping Alto**:
  - Simula un ping di **200**-**300ms** e verifica la precisione della rilevazione.

- **Prestazioni**:
  - Testa con **50**+ giocatori per confermare l'efficienza.

> [!TIP]
> Testa in un ambiente controllato con pochi giocatori prima di implementare su server affollati.

## Limitazioni e Considerazioni

- **Dipendenza dai Callback**:
  - Dipende da `OnPlayerWeaponShot` e `OnPlayerKeyStateChange`, che possono essere influenzati da lag o altri include.
- **Ping Estremo**:
  - Latenze molto alte (>**500ms**) possono richiedere regolazioni in `CBUG_D_PING_MULTIPLIER`.
- **Animazioni**:
  - La rilevazione di corsa/salto utilizza indici di animazione, che possono variare in client modificati.
- **Falsi Positivi**:
  - Nonostante sia robusto, **i falsi positivi possono verificarsi in casi rari**, come azioni legittime in sequenza rapida, il che è considerato normale.

> [!IMPORTANT]
> I **falsi positivi** sono minimizzati dalle validazioni, ma possono verificarsi in scenari atipici. Monitora le rilevazioni per regolare l'uso, se necessario.

## Licenza

Questo include è protetto sotto la Licenza Apache 2.0, che permette:

- ✔️ Uso commerciale e privato
- ✔️ Modifica del codice sorgente
- ✔️ Distribuzione del codice
- ✔️ Concessione di brevetti

### Condizioni

- Mantenere l'avviso di copyright
- Documentare le modifiche significative
- Includere una copia della licenza Apache 2.0

Per maggiori dettagli sulla licenza: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - Tutti i diritti riservati**