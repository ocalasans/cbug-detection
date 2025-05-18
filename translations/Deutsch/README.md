# Cbug Detection

Der **Cbug Detection** ist ein flexibles include für **San Andreas Multiplayer (SA-MP)**, das entwickelt wurde, um den **C-Bug-Exploit** zu erkennen, eine Technik, die es Spielern ermöglicht, schneller zu schießen als vorgesehen, indem sie Spielmechaniken ausnutzen. Dieses **Include** bietet **Serverentwicklern** ein zuverlässiges Werkzeug, um die Nutzung von **C-Bug** zu identifizieren, und ermöglicht die Integration mit Systemen wie Anti-Cheat, Verhaltensüberwachung oder benutzerdefinierten Spielregeln.

Im Kontext von **SA-MP** ist der **C-Bug** eine Praxis, die schnelle Aktionen wie das Ducken oder Wechseln der Waffe nach dem Schießen umfasst, um die Nachladezeit zu verkürzen und einen unfairen Vorteil in Kämpfen zu erlangen. Dieses **Include** erkennt diese Aktionen präzise und ist nützlich für Server im **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, Minispiele oder jedes Szenario, das eine strikte Kontrolle der Spielmechaniken erfordert. Seine Vielseitigkeit ermöglicht Anwendungen über Anti-Cheat hinaus, wie z. B. Spielanalysen oder die Erstellung einzigartiger Mechaniken.

> [!NOTE]
> Der **Cbug Detection** beschränkt sich nicht darauf, **C-Bug** zu blockieren; er kann in Überwachungssysteme, spezifische Events oder Minispiele mit benutzerdefinierten Regeln integriert werden. **Zusammengefasst entscheiden die Entwickler, welche Aktion ausgeführt werden soll, wenn ein Spieler beim Verwenden von C-Bug erkannt wird**, was eine hohe Flexibilität je nach Serverkontext ermöglicht.

## Sprachen

- Português: [README](../../)
- English: [README](../English/README.md)
- Español: [README](../Espanol/README.md)
- Français: [README](../Francais/README.md)
- Italiano: [README](../Italiano/README.md)
- Polski: [README](../Polski/README.md)
- Русский: [README](../Русский/README.md)
- Svenska: [README](../Svenska/README.md)
- Türkçe: [README](../Turkce/README.md)

## Inhaltsverzeichnis

- [Cbug Detection](#cbug-detection)
  - [Sprachen](#sprachen)
  - [Inhaltsverzeichnis](#inhaltsverzeichnis)
  - [Funktionen](#funktionen)
  - [Wie es funktioniert](#wie-es-funktioniert)
    - [Erkennungslogik](#erkennungslogik)
    - [Überwachte Ereignisse](#überwachte-ereignisse)
    - [Einstellungen (Defines)](#einstellungen-defines)
  - [Code-Struktur](#code-struktur)
  - [Installation und Konfiguration](#installation-und-konfiguration)
    - [Installation](#installation)
    - [Konfiguration](#konfiguration)
  - [Wie man es verwendet](#wie-man-es-verwendet)
    - [Aktivierung pro Spieler](#aktivierung-pro-spieler)
    - [Deaktivierung pro Spieler](#deaktivierung-pro-spieler)
    - [Statusprüfung](#statusprüfung)
    - [Integration mit Anti-Cheat](#integration-mit-anti-cheat)
    - [Protokollierung von Erkennungen](#protokollierung-von-erkennungen)
    - [Beispiel in einem Minispiel](#beispiel-in-einem-minispiel)
  - [Mögliche Anwendungen](#mögliche-anwendungen)
  - [Anpassung](#anpassung)
    - [Anpassung der Defines](#anpassung-der-defines)
    - [Anpassung des Callbacks](#anpassung-des-callbacks)
  - [Tests und Validierung](#tests-und-validierung)
  - [Einschränkungen und Überlegungen](#einschränkungen-und-überlegungen)
  - [Lizenz](#lizenz)
    - [Bedingungen:](#bedingungen)

## Funktionen

- **Umfassende C-Bug-Erkennung**: Erkennt die folgenden Varianten:
  - Classic
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Schnelle Schüsse
- **Kontrolle pro Spieler**: Aktiviert oder deaktiviert die Erkennung individuell für jeden Spieler.
- **Ping-Anpassung**: Passt Erkennungszeiten basierend auf dem Ping des Spielers an, um falsche Positivmeldungen zu minimieren.
- **Schutz vor falschen Positiven**: Validiert den Spielerzustand, die Waffe, die Munition und Animationen, um Präzision zu gewährleisten.
- **Anpassbare Einstellungen**: Definiert Erkennungsparameter über Konstanten (`defines`), obwohl die Standardwerte optimiert sind.
- **Erweiterbarer Callback**: Der Callback `CbugDetection_OnDetected` ermöglicht die Integration mit benutzerdefinierten Systemen.
- **Leicht und optimiert**: Verwendet nur `OnPlayerKeyStateChange` und `OnPlayerWeaponShot`, mit minimaler Auswirkung auf die Leistung.

## Wie es funktioniert

Das include überwacht Spieleraktionen über die Callbacks **OnPlayerKeyStateChange** und **OnPlayerWeaponShot** und verwendet ein **Verdachtspunktesystem** (`cbug_d_suspicion_score`), um C-Bug-Muster zu erkennen. Wenn die Punktzahl einen konfigurierbaren Schwellenwert (`CBUG_D_SUSPICION_THRESHOLD`) überschreitet, wird der Callback `CbugDetection_OnDetected` ausgelöst.

> [!IMPORTANT]
> Das System ist robust ausgelegt, mit strengen Validierungen, die **falsche Positivmeldungen** minimieren, **obwohl seltene Fälle auftreten können**, wie in den Einschränkungen beschrieben.

### Erkennungslogik
1. **Verdachtspunktzahl**:
   - Verdächtige Aktionen (z. B. Ducken, Waffenwechsel, schnelle Schüsse) erhöhen `cbug_d_suspicion_score` mit spezifischen Gewichtungen.
   - Beispiel: Ducken nach einem Schuss fügt **4.0** Punkte hinzu; schnelle Schüsse fügen **3.0** Punkte hinzu.
   - Die Punktzahl nimmt mit der Zeit ab (`CBUG_D_SUSPICION_DECAY = 0.5` pro Sekunde), um die Anhäufung nicht zusammenhängender Aktionen zu vermeiden.

2. **Zeitfenster**:
   - Aktionen gelten als verdächtig, wenn sie kurz nach einem Schuss erfolgen (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Schnelle Schüsse werden erkannt, wenn sie innerhalb von `CBUG_D_SHOT_INTERVAL = 200ms` erfolgen.

3. **Validierungen**:
   - Überwacht nur spezifische Waffen (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - Die Erkennung wird deaktiviert, wenn:
     - Der Spieler sich nicht zu Fuß befindet (`PLAYER_STATE_ONFOOT`).
     - Der Spieler rennt oder springt (überprüft durch Animationen).
     - Keine Munition vorhanden ist (`GetPlayerAmmo(playerid) <= 0`).
     - Die Erkennung für den Spieler deaktiviert ist (`cbug_d_player_status`).

4. **Ping-Anpassung**:
   - Die Funktion `CbugDetection_GetPingAdjtThresh` passt Zeiten basierend auf den Ping an:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Dies gewährleistet Fairness für Spieler mit höherer Latenz (z. B. **200**-**300ms**).

5. **Abklingzeit**:
   - Ein Intervall (`CBUG_D_COOLDOWN_TIME = 1500ms`) verhindert wiederholte Erkennungen für dieselbe Sequenz.

### Überwachte Ereignisse
- **OnPlayerKeyStateChange**:
  - Verfolgt gedrückte Tasten (Ducken, Rennen, Links/Rechts) und Waffenwechsel.
  - Erkennt Muster wie Ducken oder Rollen nach einem Schuss.
  - Beispiel:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Zusätzliche Überprüfungen für Rollbug, Quick C-switch, etc.
    }
    ```

- **OnPlayerWeaponShot**:
  - Überwacht Schüsse und schnelle Sequenzen.
  - Aktualisiert die Punktzahl für aufeinanderfolgende schnelle Schüsse.
  - Beispiel:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Initialisiert den Erkennungszustand des Spielers als deaktiviert und setzt Variablen zurück:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Einstellungen (Defines)
Die Defines steuern das Verhalten der Erkennung. Die Standardwerte sind optimiert und empfohlen:

| Define                        | Standardwert         | Beschreibung                                                                              |
|-------------------------------|----------------------|-------------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Punktzahl, die für eine Erkennung erforderlich ist. Höhere Werte verringern die Sensitivität. |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Punktzahlabbau pro Sekunde.                                                       |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Fenster (**ms**) für Aktionen nach einem Schuss, die als verdächtig gelten.               |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Fenster (**ms**) für die Erkennung aufeinanderfolgender schneller Schüsse.                |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Abklingzeit (**ms**) zwischen Erkennungen, um Wiederholungen zu vermeiden.               |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Multiplikator für die Ping-Anpassung bei Zeiten.                                          |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Puffer (**ms**) für die Erkennung eines Waffenwechsels nach einem Schuss.                |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Zeit (**ms**) nach der letzten Aktion, um die Punktzahl zurückzusetzen.                  |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | Überwachte Waffen (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).    |

> [!WARNING]
> Es wird nicht empfohlen, die Werte der Defines zu ändern, da die Standardwerte sorgfältig kalibriert wurden, um Präzision und Leistung zu balancieren.

## Code-Struktur

Das include ist in verschiedene Abschnitte unterteilt, jeder mit einer spezifischen Funktion:

1. **Header**:
   - Definiert den Include-Guard (`_cbug_detection_included`) und überprüft die Einbindung von `a_samp` oder `open.mp`.

2. **Einstellungen (Defines)**:
   - Definiert Konstanten für Erkennungsparameter (z. B. `CBUG_D_SUSPICION_THRESHOLD`).
   - Verwendet `#if defined`, um Konflikte bei Neudefinitionen zu vermeiden.

3. **Globale Variablen**:
   - Speichert Spielerzustände:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // Zustand pro Spieler
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Verdachtspunktzahl
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Letzter Schuss
     // ... weitere Variablen für die Nachverfolgung
     ```

4. **Hilfsfunktionen (Static Stock)**:
   - Interne Funktionen für die Erkennungslogik:
     - `CbugDetection_GetPingAdjtThresh`: Passt Zeiten nach Ping an.
     - `CbugDetection_IsCbugWeapon`: Überprüft überwachte Waffen.
     - `CbugDetection_IsJumping`: Erkennt Sprunganimationen.
     - `CbugDetection_IsRunning`: Erkennt Laufanimationen.
     - `CbugDetection_ResetVariables`: Setzt Spielervariablen zurück.
   - Beispiel:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... weitere Variablen zurücksetzen...

         return true;
     }
     ```

5. **Öffentliche Funktionen (Stock)**:
   - Funktionen zur Steuerung des Systems:
     - `CbugDetection_Player`: Aktiviert oder deaktiviert die Erkennung für einen bestimmten Spieler.
     - `CbugDetection_IsActive`: Überprüft, ob die Erkennung für einen Spieler aktiv ist.
   - Beispiel:
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

6. **Callbacks**:
   - Implementiert die Logik in `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect` und `OnPlayerDisconnect`.
   - Stellt `CbugDetection_OnDetected` für Anpassungen bereit.
   - Beispiel:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **ALS-Hooks**:
   - Integriert Standard-SA-MP-Callbacks für Kompatibilität:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... weitere ALS-Definitionen...
     ```

## Installation und Konfiguration

### Installation
1. **Include herunterladen**:
   - Platzieren Sie die Datei `cbug-detection.inc` im Verzeichnis `pawno/include`.

2. **Im Gamemode einbinden**:
   - Fügen Sie am Anfang des Gamemodes hinzu:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> Stellen Sie sicher, dass `a_samp` oder `open.mp` vor `cbug-detection.inc` eingebunden wird, da der Compiler sonst Fehler meldet.

3. **Gamemode kompilieren**:
   - Verwenden Sie den **Pawn**-Compiler, um den Gamemode zu kompilieren, und überprüfen Sie, ob keine Fehler auftreten.

### Konfiguration
- **Callback definieren**:
  - Implementieren Sie `CbugDetection_OnDetected` im Gamemode, um Erkennungen zu behandeln:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "Spieler %d wurde beim Verwenden von C-Bug erkannt!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Wie man es verwendet

Das include bietet individuelle Kontrolle über die C-Bug-Erkennung pro Spieler. Nachfolgend praktische Anwendungsbeispiele:

### Aktivierung pro Spieler
- Aktivieren Sie die Erkennung für einen bestimmten Spieler, z. B. bei einem Kampf-Event:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Deaktivierung pro Spieler
- Deaktivieren Sie die Erkennung für einen bestimmten Spieler, z. B. für Administratoren:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Statusprüfung
- Überprüfen Sie, ob die Erkennung für einen Spieler aktiv ist:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "Die Erkennung ist für Sie aktiv.");
  ```

### Integration mit Anti-Cheat
- Wenden Sie Strafen in `CbugDetection_OnDetected` an:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Spieler %s (ID: %d) wurde beim Verwenden von C-Bug erkannt!", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Protokollierung von Erkennungen
- Protokollieren Sie Erkennungen in einer Datei:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Spieler %s (ID: %d) wurde beim Verwenden von C-Bug erkannt\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Beispiel in einem Minispiel
- Aktivieren Sie die Erkennung nur für bestimmte Spieler in einem Minispiel:
  ```pawn
  CMD:startminigame(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "C-Bug-Erkennung für Sie im Minispiel aktiviert!");

      return 1;
  }

  CMD:endminigame(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "C-Bug-Erkennung für Sie deaktiviert!");

      return 1;
  }
  ```

> [!TIP]
> Verwenden Sie `CbugDetection_IsActive`, um den Erkennungszustand zu überprüfen, bevor Sie bedingte Aktionen wie Warnungen oder Strafen ausführen.

## Mögliche Anwendungen

Der **Cbug Detection** ist äußerst vielseitig und kann in verschiedenen Kontexten verwendet werden:

- **Roleplay (RP)-Server**: Erzwingen Sie Regeln gegen **C-Bug** für realistische Kämpfe.
- **TDM/DM-Server**: Gewährleisten Sie Balance in Kämpfen durch Erkennung von **C-Bug**.
- **Überwachung**: Analysieren Sie Spielerverhalten oder identifizieren Sie häufige Regelverletzer.
- **Minispiele und Events**: Kontrollieren Sie **C-Bug** nur bei bestimmten Events.
- **Benutzerdefinierte Mechaniken**: Integrieren Sie es mit Punktesystemen oder Strafen.

> [!NOTE]
> Die Fähigkeit, individuelle Einstellungen beim globalen Aktivieren oder Deaktivieren beizubehalten, macht das include ideal für Server mit dynamischen Regeln.

## Anpassung

Obwohl das include mit Standardeinstellungen optimiert ist, können die Defines oder der Callback `CbugDetection_OnDetected` angepasst werden.

> [!WARNING]
> Das Ändern der Defines oder anderer Einstellungen **wird nicht empfohlen**, da die Standardwerte getestet und für maximale Effizienz ausgeglichen sind. Änderungen können falsche Positivmeldungen, ungenaue Erkennungen oder sogar Funktionsstörungen verursachen.

### Anpassung der Defines
- **Server mit hohem Ping**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Strengere Erkennung**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Benutzerdefinierte Waffen**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Anpassung des Callbacks
- Fügen Sie erweiterte Logik hinzu:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Warnung %d: Spieler %s (ID: %d) wurde beim Verwenden von C-Bug erkannt!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Wiederholte Verwendung von C-Bug");
      
      return 1;
  }
  ```

## Tests und Validierung

Um einen korrekten Betrieb sicherzustellen, führen Sie die folgenden Tests durch:

- **C-Bug-Erkennung**:
  - Testen Sie alle aufgeführten Varianten mit überwachten Waffen.
  - Stellen Sie sicher, dass `CbugDetection_OnDetected` nach **2**-**3** Sequenzen ausgelöst wird.

- **Falsche Positivmeldungen**:
  - Führen Sie Aktionen wie Rennen, Springen oder Schießen ohne Ducken aus.
  - Überprüfen Sie, dass keine Erkennungen für nicht mit **C-Bug** zusammenhängende Aktionen erfolgen.

- **Kontrolle pro Spieler**:
  - Testen Sie `CbugDetection_Player` für die Aktivierung/Deaktivierung pro Spieler.
  - Stellen Sie sicher, dass die Erkennung nur für Spieler mit aktivem `cbug_d_player_status` erfolgt.

- **Statusprüfung**:
  - Verwenden Sie `CbugDetection_IsActive`, um den Status pro Spieler zu bestätigen.

- **OnPlayerConnect**:
  - Überprüfen Sie, ob neue Spieler mit deaktivierter Erkennung starten (`cbug_d_player_status = false`).

- **Hoher Ping**:
  - Simulieren Sie einen Ping von **200**-**300ms** und überprüfen Sie die Erkennungsgenauigkeit.

- **Leistung**:
  - Testen Sie mit **50+** Spielern, um die Effizienz zu bestätigen.

> [!TIP]
> Testen Sie in einer kontrollierten Umgebung mit wenigen Spielern, bevor Sie es auf stark frequentierten Servern einsetzen.

## Einschränkungen und Überlegungen

- **Abhängigkeit von Callbacks**:
  - Hängt von `OnPlayerWeaponShot` und `OnPlayerKeyStateChange` ab, die durch Lag oder andere Includes beeinflusst werden können.
- **Extremer Ping**:
  - Sehr hohe Latenzen (>**500ms**) können Anpassungen an `CBUG_D_PING_MULTIPLIER` erfordern.
- **Animationen**:
  - Die Erkennung von Lauf-/Sprunganimationen verwendet Animationsindizes, die bei modifizierten Clients variieren können.
- **Falsche Positivmeldungen**:
  - Trotz Robustheit **können in seltenen Fällen falsche Positivmeldungen auftreten**, z. B. bei legitimen schnellen Aktionen, was als normal angesehen wird.

> [!IMPORTANT]
> **Falsche Positivmeldungen** werden durch Validierungen minimiert, können aber in untypischen Szenarien auftreten. Überwachen Sie Erkennungen, um den Einsatz bei Bedarf anzupassen.

## Lizenz

Dieses include steht unter der Apache 2.0 Lizenz, die Folgendes erlaubt:

- ✔️ Kommerzielle und private Nutzung
- ✔️ Modifikation des Quellcodes
- ✔️ Verteilung des Codes
- ✔️ Patenterteilung

### Bedingungen:

- Urheberrechtshinweis beibehalten
- Wesentliche Änderungen dokumentieren
- Kopie der Apache 2.0 Lizenz beifügen

Weitere Details zur Lizenz: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - Alle Rechte vorbehalten**