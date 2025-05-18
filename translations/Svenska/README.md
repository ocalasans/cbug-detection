# Cbug Detection

**Cbug Detection** är ett flexibelt include för **San Andreas Multiplayer (SA-MP)** utformat för att detektera **C-Bug-exploiten**, en teknik som gör det möjligt för spelare att skjuta snabbare än avsett genom att utnyttja spelets mekanik. Detta **include** ger **serverutvecklare** ett pålitligt verktyg för att identifiera användningen av **C-Bug**, vilket möjliggör integration med system som anti-cheat, beteendeövervakning eller anpassade spelregler.

Inom **SA-MP** är **C-Bug** en praxis som innebär snabba handlingar, som att huka eller byta vapen, efter att ha avfyrat vissa vapen, vilket minskar omladdningstiden och ger en orättvis fördel i strider. Detta **include** detekterar dessa handlingar med precision och är användbart i servrar för **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, minispel eller andra scenarier som kräver strikt kontroll över spelmekaniken. Dess mångsidighet gör det möjligt att använda det utöver anti-cheat, till exempel för spelanalys eller skapande av unika mekaniker.

> [!NOTE]
> **Cbug Detection** är inte begränsat till att blockera **C-Bug**; det kan integreras i övervakningssystem, specifika evenemang eller minispel med anpassade regler. **Kort sagt, det är utvecklarna som bestämmer vilken åtgärd som ska vidtas när en spelare detekteras använda C-Bug**, vilket ger stor flexibilitet i tillämpningen av detektionen beroende på serverns kontext.

## Språk

- Português: [README](../../)
- Deutsch: [README](../Deutsch/README.md)
- English: [README](../English/README.md)
- Español: [README](../Espanol/README.md)
- Français: [README](../Francais/README.md)
- Italiano: [README](../Italiano/README.md)
- Polski: [README](../Polski/README.md)
- Русский: [README](../Русский/README.md)
- Türkçe: [README](../Turkce/README.md)

## Innehållsförteckning

- [Cbug Detection](#cbug-detection)
  - [Språk](#språk)
  - [Innehållsförteckning](#innehållsförteckning)
  - [Funktioner](#funktioner)
  - [Hur det fungerar](#hur-det-fungerar)
    - [Detektionslogik](#detektionslogik)
    - [Övervakade händelser](#övervakade-händelser)
    - [Konfigurationer (Defines)](#konfigurationer-defines)
  - [Kodstruktur](#kodstruktur)
  - [Installation och konfiguration](#installation-och-konfiguration)
    - [Installation](#installation)
    - [Konfiguration](#konfiguration)
  - [Hur man använder](#hur-man-använder)
    - [Aktivering per spelare](#aktivering-per-spelare)
    - [Inaktivering per spelare](#inaktivering-per-spelare)
    - [Kontroll av status](#kontroll-av-status)
    - [Integration med Anti-Cheat](#integration-med-anti-cheat)
    - [Loggning av detektioner](#loggning-av-detektioner)
    - [Exempel i minispel](#exempel-i-minispel)
  - [Möjliga tillämpningar](#möjliga-tillämpningar)
  - [Anpassning](#anpassning)
    - [Justering av Defines](#justering-av-defines)
    - [Anpassning av Callback](#anpassning-av-callback)
  - [Tester och validering](#tester-och-validering)
  - [Begränsningar och överväganden](#begränsningar-och-överväganden)
  - [Licens](#licens)
    - [Villkor:](#villkor)

## Funktioner

- **Omfattande C-Bug-detektion**: Identifierar följande varianter:
  - Classic
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Snabba skott
- **Kontroll per spelare**: Aktiverar eller inaktiverar detektion individuellt för varje spelare.
- **Anpassning efter ping**: Anpassar detektionstider baserat på spelarens ping för att minimera falska positiva resultat.
- **Skydd mot falska positiva**: Validerar spelarens status, vapen, ammunition och animationer för att säkerställa noggrannhet.
- **Anpassningsbara inställningar**: Definierar detektionsparametrar via konstanter (`defines`), även om standardvärdena är optimerade.
- **Utökningsbar Callback**: Callbacken `CbugDetection_OnDetected` möjliggör integration med anpassade system.
- **Lätt och optimerat**: Använder endast `OnPlayerKeyStateChange` och `OnPlayerWeaponShot`, med minimal påverkan på prestanda.

## Hur det fungerar

Includet övervakar spelares handlingar via callbacks **OnPlayerKeyStateChange** och **OnPlayerWeaponShot**, och använder ett system med **misstänkepoäng** (`cbug_d_suspicion_score`) för att identifiera C-Bug-mönster. När poängen överstiger ett konfigurerbart tröskelvärde (`CBUG_D_SUSPICION_THRESHOLD`) aktiveras callbacken `CbugDetection_OnDetected`.

> [!IMPORTANT]
> Systemet är utformat för att vara robust, med strikta valideringar som minimerar **falska positiva**, **även om sällsynta fall kan förekomma**, som beskrivs i avsnittet om begränsningar.

### Detektionslogik
1. **Misstänkepoäng**:
   - Misstänkta handlingar (t.ex. huka, byta vapen, snabba skott) ökar `cbug_d_suspicion_score` med specifika vikter.
   - Exempel: Att huka efter att ha skjutit lägger till **4.0** poäng; snabba skott lägger till **3.0** poäng.
   - Poängen minskar över tid (`CBUG_D_SUSPICION_DECAY = 0.5` per sekund) för att undvika ackumulering av orelaterade handlingar.

2. **Tidsfönster**:
   - Handlingar anses misstänkta om de inträffar strax efter ett skott (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Snabba skott detekteras om de inträffar inom `CBUG_D_SHOT_INTERVAL = 200ms`.

3. **Valideringar**:
   - Övervakar endast specifika vapen (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - Detektionen inaktiveras om:
     - Spelaren inte är till fots (`PLAYER_STATE_ONFOOT`).
     - Spelaren springer eller hoppar (kontrolleras via animationer).
     - Ingen ammunition finns (`GetPlayerAmmo(playerid) <= 0`).
     - Detektionen är inaktiverad för spelaren (`cbug_d_player_status`).

4. **Anpassning efter ping**:
   - Funktionen `CbugDetection_GetPingAdjtThresh` justerar tiderna baserat på ping:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Detta säkerställer rättvisa för spelare med högre latens (t.ex. **200**-**300ms**).

5. **Cooldown**:
   - Ett intervall (`CBUG_D_COOLDOWN_TIME = 1500ms`) förhindrar upprepade detektioner för samma sekvens.

### Övervakade händelser
- **OnPlayerKeyStateChange**:
  - Spårar tryckta tangenter (huka, springa, vänster/höger) och vapenbyten.
  - Detekterar mönster som att huka eller rulla efter att ha skjutit.
  - Exempel:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Ytterligare kontroller för Rollbug, Quick C-switch, etc.
    }
    ```

- **OnPlayerWeaponShot**:
  - Övervakar skott och snabba sekvenser.
  - Uppdaterar poängen för på varandra följande snabba skott.
  - Exempel:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Initierar spelarens detektionsstatus som inaktiverad och återställer variabler:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Konfigurationer (Defines)
Defines styr detektionens beteende. Standardvärdena är optimerade och rekommenderade:

| Define                        | Standardvärde        | Beskrivning                                                                            |
|-------------------------------|---------------------|----------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                | Poäng som krävs för detektion. Högre värden minskar känsligheten.                      |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                 | Poängminskning per sekund.                                                            |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                | Fönster (**ms**) för handlingar efter skott som anses misstänkta.                      |
| `CBUG_D_SHOT_INTERVAL`        | 200                 | Fönster (**ms**) för att detektera på varandra följande snabba skott.                  |
| `CBUG_D_COOLDOWN_TIME`        | 1500                | Cooldown (**ms**) mellan detektioner för att undvika upprepningar.                     |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                | Multiplikator för pingjustering av tider.                                              |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                 | Buffert (**ms**) för detektion av vapenbyte efter skott.                               |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                | Tid (**ms**) efter sista handling för att återställa poäng.                            |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34}| Vapen som övervakas (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).|

> [!WARNING]
> Det rekommenderas inte att ändra definiernas värden, eftersom standardvärdena är noggrant kalibrerade för att balansera noggrannhet och prestanda.

## Kodstruktur

Includet är organiserat i tydliga sektioner, var och en med en specifik funktion:

1. **Huvud**:
   - Definierar include guard (`_cbug_detection_included`) och kontrollerar inkluderingen av `a_samp` eller `open.mp`.

2. **Konfigurationer (Defines)**:
   - Definierar konstanter för detektionsparametrar (t.ex. `CBUG_D_SUSPICION_THRESHOLD`).
   - Använder `#if defined` för att undvika konflikter vid omdefinition.

3. **Globala variabler**:
   - Lagrar spelares tillstånd:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // Status per spelare
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Misstänkepoäng
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Sista skott
     // ... andra variabler för spårning
     ```

4. **Verktygsfunktioner (Static Stock)**:
   - Interna funktioner för detektionslogik:
     - `CbugDetection_GetPingAdjtThresh`: Justerar tider efter ping.
     - `CbugDetection_IsCbugWeapon`: Kontrollerar övervakade vapen.
     - `CbugDetection_IsJumping`: Detekterar hopp-animationer.
     - `CbugDetection_IsRunning`: Detekterar löp-animationer.
     - `CbugDetection_ResetVariables`: Återställer spelarens variabler.
   - Exempel:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... andra variabler att återställa...

         return true;
     }
     ```

5. **Publika funktioner (Stock)**:
   - Funktioner för systemkontroll:
     - `CbugDetection_Player`: Aktiverar eller inaktiverar detektion för en specifik spelare.
     - `CbugDetection_IsActive`: Kontrollerar om detektionen är aktiv för en spelare.
   - Exempel:
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
   - Implementerar logik i `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect` och `OnPlayerDisconnect`.
   - Tillhandahåller `CbugDetection_OnDetected` för anpassning.
   - Exempel:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **ALS Hooks**:
   - Integrerar standard SA-MP-callbacks för kompatibilitet:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... andra ALS att definiera...
     ```

## Installation och konfiguration

### Installation
1. **Ladda ner includet**:
   - Placera filen `cbug-detection.inc` i katalogen `pawno/include`.

2. **Inkludera i Gamemode**:
   - Lägg till i början av gamemode:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> Se till att inkludera `a_samp` eller `open.mp` före `cbug-detection.inc`, annars rapporterar kompilatorn fel.

3. **Kompilera Gamemode**:
   - Använd **Pawn**-kompilatorn för att kompilera gamemode och verifiera att inga fel uppstår.

### Konfiguration
- **Definiera Callback**:
  - Implementera `CbugDetection_OnDetected` i gamemode för att hantera detektioner:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "Spelare %d detekterad använda C-Bug!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Hur man använder

Includet erbjuder individuell kontroll över C-Bug-detektion per spelare. Nedan följer praktiska exempel på användning:

### Aktivering per spelare
- Aktivera för en specifik spelare, t.ex. i ett stridsevenemang:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Inaktivering per spelare
- Inaktivera för en specifik spelare, t.ex. för administratörer:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Kontroll av status
- Kontrollera om detektionen är aktiv för en spelare:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "Detektion är aktiv för dig.");
  ```

### Integration med Anti-Cheat
- Tillämpa straff i `CbugDetection_OnDetected`:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Spelare %s (ID: %d) detekterad använda C-Bug!", name, playerid);
      SendClientMessageToAll(-1, string);
      
      Kick(playerid);

      return 1;
  }
  ```

### Loggning av detektioner
- Logga detektioner i en fil:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Spelare %s (ID: %d) detekterad använda C-Bug\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Exempel i minispel
- Aktivera detektionen endast för specifika spelare i ett minispel:
  ```pawn
  CMD:startaminispel(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "C-Bug-detektion aktiverad för dig i minispelet!");

      return 1;
  }

  CMD:avslutaminispel(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "C-Bug-detektion inaktiverad för dig!");

      return 1;
  }
  ```

> [!TIP]
> Använd `CbugDetection_IsActive` för att kontrollera detektionsstatus innan du utför villkorliga åtgärder, som varningar eller straff.

## Möjliga tillämpningar

**Cbug Detection** är mycket mångsidigt och kan användas i olika sammanhang:

- **Roleplay-servrar (RP)**: Förstärk regler mot **C-Bug** för realistiska strider.
- **TDM/DM-servrar**: Säkerställ balans i strider genom att detektera **C-Bug**.
- **Övervakning**: Analysera spelares beteende eller identifiera frekventa regelbrytare.
- **Minispel och evenemang**: Kontrollera **C-Bug** endast i specifika evenemang.
- **Anpassade mekaniker**: Integrera med poängsystem eller straffmekanismer.

> [!NOTE]
> Förmågan att bevara individuella inställningar vid global aktivering eller inaktivering gör includet idealiskt för servrar med dynamiska regler.

## Anpassning

Även om includet är optimerat med standardinställningar är det möjligt att justera definier eller callbacken `CbugDetection_OnDetected`.

> [!WARNING]
> Att ändra definier eller andra inställningar **rekommenderas inte**, eftersom standardvärdena är testade och balanserade för maximal effektivitet. Ändringar kan orsaka falska positiva, inexakta detektioner eller till och med förhindra korrekt funktion.

### Justering av Defines
- **Servrar med hög ping**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Strängare detektion**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Anpassade vapen**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Anpassning av Callback
- Lägg till avancerad logik:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Varning %d: Spelare %s (ID: %d) detekterad använda C-Bug!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Upprepad användning av C-Bug");
      
      return 1;
  }
  ```

## Tester och validering

För att säkerställa korrekt funktion, utför följande tester:

- **C-Bug-detektion**:
  - Testa alla listade varianter med övervakade vapen.
  - Bekräfta att `CbugDetection_OnDetected` aktiveras efter **2**-**3** sekvenser.

- **Falska positiva**:
  - Utför handlingar som att springa, hoppa eller skjuta utan att huka.
  - Verifiera att inga detektioner sker för handlingar som inte är relaterade till **C-Bug**.

- **Kontroll per spelare**:
  - Testa `CbugDetection_Player` för aktivering/inaktivering per spelare.
  - Bekräfta att detektion endast sker för spelare med aktiv `cbug_d_player_status`.

- **Statuskontroll**:
  - Använd `CbugDetection_IsActive` för att bekräfta status per spelare.

- **OnPlayerConnect**:
  - Verifiera att nya spelare börjar med detektion inaktiverad (`cbug_d_player_status = false`).

- **Hög ping**:
  - Simulera ping på **200**-**300ms** och kontrollera detektionens noggrannhet.

- **Prestanda**:
  - Testa med **50+** spelare för att bekräfta effektivitet.

> [!TIP]
> Testa i en kontrollerad miljö med få spelare innan du implementerar på fullsatta servrar.

## Begränsningar och överväganden

- **Beroende av Callbacks**:
  - Beror på `OnPlayerWeaponShot` och `OnPlayerKeyStateChange`, som kan påverkas av lag eller andra includes.
- **Extrem ping**:
  - Mycket höga latenser (>**500ms**) kan kräva justeringar i `CBUG_D_PING_MULTIPLIER`.
- **Animationer**:
  - Detektion av löpning/hopp använder animationsindex, som kan variera i modifierade klienter.
- **Falska positiva**:
  - Trots robusthet kan **falska positiva förekomma i sällsynta fall**, som legitima snabba sekvenser, vilket anses normalt.

> [!IMPORTANT]
> **Falska positiva** minimeras genom valideringar, men kan förekomma i atypiska scenarier. Övervaka detektioner för att justera användningen vid behov.

## Licens

Detta include är skyddat under Apache 2.0-licensen, som tillåter:

- ✔️ Kommersiell och privat användning
- ✔️ Modifiering av källkoden
- ✔️ Distribution av koden
- ✔️ Patentbeviljande

### Villkor:

- Behåll upphovsrättsmeddelandet
- Dokumentera betydande ändringar
- Inkludera en kopia av Apache 2.0-licensen

För mer information om licensen: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - Alla rättigheter förbehållna**