# Cbug Detection

**Cbug Detection** to elastyczny include dla **San Andreas Multiplayer (SA-MP)** zaprojektowany do wykrywania exploita **C-Bug**, techniki pozwalającej graczom strzelać szybciej niż przewidziano, wykorzystując mechaniki gry. Ten **include** dostarcza **deweloperom serwerów** niezawodne narzędzie do identyfikacji użycia **C-Bug**, umożliwiając integrację z systemami antycheat, monitorowaniem zachowania lub niestandardowymi zasadami rozgrywki.

W kontekście **SA-MP**, **C-Bug** to praktyka polegająca na szybkich akcjach, takich jak kucanie lub zmiana broni, po oddaniu strzału z niektórych broni, co skraca czas przeładowania i daje nieuczciwą przewagę w walce. Ten **include** precyzyjnie wykrywa te akcje, będąc użytecznym na serwerach **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, w minigrach lub w dowolnym scenariuszu wymagającym ścisłej kontroli mechanik gry. Jego wszechstronność pozwala na zastosowania poza antycheatem, takie jak analiza rozgrywki lub tworzenie unikalnych mechanik.

> [!NOTE]
> **Cbug Detection** nie ogranicza się do blokowania **C-Bug**; może być zintegrowany z systemami monitorowania, specjalnymi wydarzeniami lub minigrami z niestandardowymi zasadami. **Krótko mówiąc, to deweloperzy decydują, jakie działania powinny zostać podjęte, gdy gracz zostanie wykryty przy użyciu C-Bug**, co zapewnia szeroką elastyczność w zastosowaniu wykrywania w zależności od kontekstu serwera.

## Języki

- Português: [README](../../)
- Deutsch: [README](../Deutsch/README.md)
- English: [README](../English/README.md)
- Español: [README](../Espanol/README.md)
- Français: [README](../Francais/README.md)
- Italiano: [README](../Italiano/README.md)
- Русский: [README](../Русский/README.md)
- Svenska: [README](../Svenska/README.md)
- Türkçe: [README](../Turkce/README.md)

## Spis treści

- [Cbug Detection](#cbug-detection)
  - [Języki](#języki)
  - [Spis treści](#spis-treści)
  - [Funkcjonalności](#funkcjonalności)
  - [Jak to działa](#jak-to-działa)
    - [Logika wykrywania](#logika-wykrywania)
    - [Monitorowane zdarzenia](#monitorowane-zdarzenia)
    - [Ustawienia (Defines)](#ustawienia-defines)
  - [Struktura kodu](#struktura-kodu)
  - [Instalacja i konfiguracja](#instalacja-i-konfiguracja)
    - [Instalacja](#instalacja)
    - [Konfiguracja](#konfiguracja)
  - [Jak używać](#jak-używać)
    - [Aktywacja dla gracza](#aktywacja-dla-gracza)
    - [Dezaktywacja dla gracza](#dezaktywacja-dla-gracza)
    - [Sprawdzanie stanu](#sprawdzanie-stanu)
    - [Integracja z antycheatem](#integracja-z-antycheatem)
    - [Rejestracja wykryć](#rejestracja-wykryć)
    - [Przykład w minigrze](#przykład-w-minigrze)
  - [Możliwe zastosowania](#możliwe-zastosowania)
  - [Personalizacja](#personalizacja)
    - [Dostosowanie defines](#dostosowanie-defines)
    - [Personalizacja callback](#personalizacja-callback)
  - [Testy i walidacja](#testy-i-walidacja)
  - [Ograniczenia i uwagi](#ograniczenia-i-uwagi)
  - [Licencja](#licencja)
    - [Warunki:](#warunki)

## Funkcjonalności

- **Kompleksowe wykrywanie C-Bug**: Rozpoznaje następujące warianty:
  - Klasyczny
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Szybkie strzały
- **Kontrola dla gracza**: Włącza lub wyłącza wykrywanie indywidualnie dla każdego gracza.
- **Dostosowanie do pingu**: Adaptuje czasy wykrywania na podstawie pingu gracza, minimalizując fałszywe pozytywy.
- **Ochrona przed fałszywymi pozytywami**: Weryfikuje stan gracza, broń, amunicję i animacje, aby zapewnić dokładność.
- **Konfigurowalne ustawienia**: Definiuje parametry wykrywania za pomocą stałych (`defines`), choć wartości domyślne są zoptymalizowane.
- **Rozszerzalny callback**: Callback `CbugDetection_OnDetected` umożliwia integrację z niestandardowymi systemami.
- **Lekki i zoptymalizowany**: Używa jedynie `OnPlayerKeyStateChange` i `OnPlayerWeaponShot`, z minimalnym wpływem na wydajność.

## Jak to działa

Include monitoruje akcje graczy za pomocą callbacków **OnPlayerKeyStateChange** i **OnPlayerWeaponShot**, wykorzystując system **punktacji podejrzliwości** (`cbug_d_suspicion_score`) do identyfikacji wzorców C-Bug. Gdy punktacja przekroczy konfigurowalny próg (`CBUG_D_SUSPICION_THRESHOLD`), wywoływany jest callback `CbugDetection_OnDetected`.

> [!IMPORTANT]
> System jest zaprojektowany jako solidny, z rygorystycznymi walidacjami minimalizującymi **fałszywe pozytywy**, **choć w rzadkich przypadkach mogą one wystąpić**, jak opisano w sekcji ograniczeń.

### Logika wykrywania
1. **Punktacja podejrzliwości**:
   - Podejrzane akcje (np. kucanie, zmiana broni, szybkie strzały) zwiększają `cbug_d_suspicion_score` z określonymi wagami.
   - Przykład: Kucanie po strzale dodaje **4.0** punktów; szybkie strzały dodają **3.0** punktów.
   - Punktacja zmniejsza się z czasem (`CBUG_D_SUSPICION_DECAY = 0.5` na sekundę), aby uniknąć kumulacji niepowiązanych akcji.

2. **Okna czasowe**:
   - Akcje są uznawane za podejrzane, jeśli występują krótko po strzale (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Szybkie strzały są wykrywane, jeśli występują w czasie `CBUG_D_SHOT_INTERVAL = 200ms`.

3. **Walidacje**:
   - Monitoruje tylko określone bronie (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - Wykrywanie jest wyłączane, jeśli:
     - Gracz nie znajduje się na nogach (`PLAYER_STATE_ONFOOT`).
     - Gracz biega lub skacze (sprawdzane przez animacje).
     - Brak amunicji (`GetPlayerAmmo(playerid) <= 0`).
     - Wykrywanie jest wyłączone dla gracza (`cbug_d_player_status`).

4. **Dostosowanie do pingu**:
   - Funkcja `CbugDetection_GetPingAdjtThresh` dostosowuje czasy na podstawie pingu:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Zapewnia to sprawiedliwość dla graczy z wyższą latencją (np. **200**-**300ms**).

5. **Cooldown**:
   - Interwał (`CBUG_D_COOLDOWN_TIME = 1500ms`) zapobiega wielokrotnym wykryciom dla tej samej sekwencji.

### Monitorowane zdarzenia
- **OnPlayerKeyStateChange**:
  - Śledzi wciśnięte klawisze (kucanie, bieganie, lewo/prawo) i zmiany broni.
  - Wykrywa wzorce, takie jak kucanie lub toczenie się po strzale.
  - Przykład:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Dodatkowe sprawdzenia dla Rollbug, Quick C-switch itp.
    }
    ```

- **OnPlayerWeaponShot**:
  - Monitoruje strzały i szybkie sekwencje.
  - Aktualizuje punktację dla szybkich, kolejnych strzałów.
  - Przykład:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Inicjalizuje stan wykrywania gracza jako wyłączony i resetuje zmienne:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Ustawienia (Defines)
Defines kontrolują zachowanie wykrywania. Domyślne wartości są zoptymalizowane i zalecane:

| Define                        | Wartość domyślna     | Opis                                                                                   |
|-------------------------------|----------------------|----------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Punktacja wymagana do wykrycia. Wyższe wartości zmniejszają czułość.                   |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Spadek punktacji na sekundę.                                                           |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Okno (**ms**) dla akcji po strzale uznawanych za podejrzane.                           |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Okno (**ms**) do wykrywania szybkich, kolejnych strzałów.                              |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Cooldown (**ms**) między wykryciami, aby uniknąć powtórek.                             |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Mnożnik dla dostosowania pingu w czasach.                                              |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Bufor (**ms**) dla wykrywania zmiany broni po strzale.                                 |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Czas (**ms**) po ostatniej akcji do zresetowania punktacji.                            |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | Monitorowane bronie (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**). |

> [!WARNING]
> Nie zaleca się zmiany wartości defines, ponieważ domyślne wartości zostały starannie skalibrowane, aby zrównoważyć dokładność i wydajność.

## Struktura kodu

Include jest zorganizowany w oddzielne sekcje, każda z określoną funkcją:

1. **Nagłówek**:
   - Definiuje include guard (`_cbug_detection_included`) i sprawdza włączenie `a_samp` lub `open.mp`.

2. **Ustawienia (Defines)**:
   - Definiuje stałe dla parametrów wykrywania (np. `CBUG_D_SUSPICION_THRESHOLD`).
   - Używa `#if defined`, aby uniknąć konfliktów ponownego definiowania.

3. **Zmienne globalne**:
   - Przechowuje stany graczy:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // Stan dla gracza
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Punktacja podejrzliwości
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Ostatni strzał
     // ... inne zmienne do śledzenia
     ```

4. **Funkcje użytkowe (Static Stock)**:
   - Wewnętrzne funkcje dla logiki wykrywania:
     - `CbugDetection_GetPingAdjtThresh`: Dostosowuje czasy dla pingu.
     - `CbugDetection_IsCbugWeapon`: Sprawdza monitorowane bronie.
     - `CbugDetection_IsJumping`: Wykrywa animacje skoku.
     - `CbugDetection_IsRunning`: Wykrywa animacje biegu.
     - `CbugDetection_ResetVariables`: Resetuje zmienne gracza.
   - Przykład:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... inne zmienne do zresetowania...

         return true;
     }
     ```

5. **Funkcje publiczne (Stock)**:
   - Funkcje do kontroli systemu:
     - `CbugDetection_Player`: Włącza lub wyłącza wykrywanie dla konkretnego gracza.
     - `CbugDetection_IsActive`: Sprawdza, czy wykrywanie jest aktywne dla gracza.
   - Przykład:
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

6. **Callbacki**:
   - Implementuje logikę w `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect` i `OnPlayerDisconnect`.
   - Dostarcza `CbugDetection_OnDetected` dla personalizacji.
   - Przykład:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **Haki ALS**:
   - Integruje standardowe callbacki SA-MP dla kompatybilności:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... inne haki ALS do zdefiniowania...
     ```

## Instalacja i konfiguracja

### Instalacja
1. **Pobierz include**:
   - Umieść plik `cbug-detection.inc` w katalogu `pawno/include`.

2. **Włącz w gamemode**:
   - Dodaj na początku gamemode:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> Upewnij się, że `a_samp` lub `open.mp` jest włączone przed `cbug-detection.inc`, w przeciwnym razie kompilator zgłosi błędy.

3. **Skompiluj gamemode**:
   - Użyj kompilatora **Pawn**, aby skompilować gamemode, sprawdzając brak błędów.

### Konfiguracja
- **Zdefiniuj callback**:
  - Zaimplementuj `CbugDetection_OnDetected` w gamemode, aby obsługiwać wykrycia:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "Gracz %d wykryty przy użyciu C-Bug!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Jak używać

Include oferuje indywidualną kontrolę nad wykrywaniem C-Bug dla każdego gracza. Poniżej praktyczne przykłady użycia:

### Aktywacja dla gracza
- Włącz dla konkretnego gracza, np. podczas wydarzenia bojowego:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Dezaktywacja dla gracza
- Wyłącz dla konkretnego gracza, np. dla administratorów:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Sprawdzanie stanu
- Sprawdź, czy wykrywanie jest aktywne dla gracza:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "Wykrywanie jest aktywne dla ciebie.");
  ```

### Integracja z antycheatem
- Zastosuj kary w `CbugDetection_OnDetected`:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Gracz %s (ID: %d) wykryty przy użyciu C-Bug!", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Rejestracja wykryć
- Zapisz wykrycia do pliku:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Gracz %s (ID: %d) wykryty przy użyciu C-Bug\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Przykład w minigrze
- Włącz wykrywanie tylko dla określonych graczy w minigrze:
  ```pawn
  CMD:rozpocznijminigre(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "Wykrywanie C-Bug aktywowane dla ciebie w minigrze!");

      return 1;
  }

  CMD:zakonczminigre(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "Wykrywanie C-Bug wyłączone dla ciebie!");

      return 1;
  }
  ```

> [!TIP]
> Używaj `CbugDetection_IsActive`, aby sprawdzić stan wykrywania przed wykonaniem działań warunkowych, takich jak alerty lub kary.

## Możliwe zastosowania

**Cbug Detection** jest bardzo wszechstronny i może być używany w różnych kontekstach:

- **Serwery Roleplay (RP)**: Wzmacniaj zasady przeciwko **C-Bug** dla realistycznych walk.
- **Serwery TDM/DM**: Zapewniaj równowagę w walkach, wykrywając **C-Bug**.
- **Monitorowanie**: Analizuj zachowanie graczy lub identyfikuj częstych naruszycieli.
- **Minigry i wydarzenia**: Kontroluj **C-Bug** tylko w określonych wydarzeniach.
- **Niestandardowe mechaniki**: Integruj z systemami punktacji lub kar.

> [!NOTE]
> Możliwość zachowania indywidualnych ustawień przy włączaniu lub wyłączaniu globalnie sprawia, że include jest idealny dla serwerów z dynamicznymi zasadami.

## Personalizacja

Chociaż include jest zoptymalizowany z domyślnymi ustawieniami, możliwe jest dostosowanie defines lub callback `CbugDetection_OnDetected`.

> [!WARNING]
> Zmiana defines lub innych ustawień **nie jest zalecana**, ponieważ domyślne wartości zostały przetestowane i zrównoważone dla maksymalnej skuteczności. Modyfikacje mogą powodować fałszywe pozytywy, niedokładne wykrycia lub nawet uniemożliwić prawidłowe działanie.

### Dostosowanie defines
- **Serwery z wysokim pingiem**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Bardziej rygorystyczne wykrywanie**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Niestandardowe bronie**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Personalizacja callback
- Dodaj zaawansowaną logikę:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Ostrzeżenie %d: Gracz %s (ID: %d) wykryty przy użyciu C-Bug!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Powtarzające się użycie C-Bug");
      
      return 1;
  }
  ```

## Testy i walidacja

Aby zapewnić prawidłowe działanie, wykonaj następujące testy:

- **Wykrywanie C-Bug**:
  - Przetestuj wszystkie wymienione warianty z monitorowanymi broniami.
  - Potwierdź, że `CbugDetection_OnDetected` jest wywoływany po **2**-**3** sekwencjach.

- **Fałszywe pozytywy**:
  - Wykonaj akcje, takie jak bieganie, skakanie lub strzelanie bez kucania.
  - Sprawdź, że nie ma wykryć dla akcji niezwiązanych z **C-Bug**.

- **Kontrola dla gracza**:
  - Przetestuj `CbugDetection_Player` dla włączania/wyłączania dla gracza.
  - Potwierdź, że wykrywanie działa tylko dla graczy z aktywnym `cbug_d_player_status`.

- **Sprawdzanie stanu**:
  - Użyj `CbugDetection_IsActive`, aby potwierdzić stan dla gracza.

- **OnPlayerConnect**:
  - Sprawdź, czy nowi gracze zaczynają z wyłączonym wykrywaniem (`cbug_d_player_status = false`).

- **Wysoki ping**:
  - Zasymuluj ping **200**-**300ms** i sprawdź dokładność wykrywania.

- **Wydajność**:
  - Przetestuj z **50**+ graczami, aby potwierdzić efektywność.

> [!TIP]
> Testuj w kontrolowanym środowisku z niewielką liczbą graczy przed wdrożeniem na zatłoczonych serwerach.

## Ograniczenia i uwagi

- **Zależność od callbacków**:
  - Zależy od `OnPlayerWeaponShot` i `OnPlayerKeyStateChange`, które mogą być wpływane przez lag lub inne includy.
- **Ekstremalny ping**:
  - Bardzo wysokie opóźnienia (>**500ms**) mogą wymagać dostosowania `CBUG_D_PING_MULTIPLIER`.
- **Animacje**:
  - Wykrywanie biegu/skoku używa indeksów animacji, które mogą się różnić w zmodyfikowanych klientach.
- **Fałszywe pozytywy**:
  - Pomimo solidności, **fałszywe pozytywy mogą wystąpić w rzadkich przypadkach**, takich jak legalne szybkie sekwencje akcji, co jest uznawane za normalne.

> [!IMPORTANT]
> **Fałszywe pozytywy** są minimalizowane przez walidacje, ale mogą wystąpić w nietypowych scenariuszach. Monitoruj wykrycia, aby dostosować użycie, jeśli to konieczne.

## Licencja

Ten include jest chroniony Licencją Apache 2.0, która pozwala na:

- ✔️ Użycie komercyjne i prywatne
- ✔️ Modyfikację kodu źródłowego
- ✔️ Dystrybucję kodu
- ✔️ Udzielanie patentów

### Warunki:

- Zachowanie informacji o prawach autorskich
- Dokumentowanie znaczących zmian
- Dołączenie kopii licencji Apache 2.0

Więcej szczegółów o licencji: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - Wszelkie prawa zastrzeżone**