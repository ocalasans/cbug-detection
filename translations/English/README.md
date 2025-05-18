# Cbug Detection

**Cbug Detection** is a flexible include for **San Andreas Multiplayer (SA-MP)** designed to detect the **C-Bug exploit**, a technique that allows players to shoot faster than intended by exploiting game mechanics. This **include** provides **server developers** with a reliable tool to identify the use of **C-Bug**, enabling integration with systems such as anti-cheat, behavior monitoring, or custom gameplay rules.

In the context of **SA-MP**, the **C-Bug** is a practice that involves rapid actions, such as crouching or switching weapons, after firing certain weapons, reducing reload time and granting an unfair advantage in combat. This **include** detects these actions accurately, being useful in **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)** servers, minigames, or any scenario requiring strict control of game mechanics. Its versatility allows uses beyond anti-cheat, such as gameplay analysis or creating unique mechanics.

> [!NOTE]
> **Cbug Detection** is not limited to blocking **C-Bug**; it can be integrated into monitoring systems, specific events, or minigames with custom rules. **In summary, developers decide what action should occur when a player is detected using C-Bug**, allowing wide flexibility in applying detection based on the server's context.

## Languages

- Portuguese: [README](../../)
- Deutsch: [README](../Deutsch/README.md)
- Español: [README](../Espanol/README.md)
- Français: [README](../Francais/README.md)
- Italiano: [README](../Italiano/README.md)
- Polski: [README](../Polski/README.md)
- Русский: [README](../Русский/README.md)
- Svenska: [README](../Svenska/README.md)
- Türkçe: [README](../Turkce/README.md)

## Table of Contents

- [Cbug Detection](#cbug-detection)
  - [Languages](#languages)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [How It Works](#how-it-works)
    - [Detection Logic](#detection-logic)
    - [Monitored Events](#monitored-events)
    - [Settings (Defines)](#settings-defines)
  - [Code Structure](#code-structure)
  - [Installation and Configuration](#installation-and-configuration)
    - [Installation](#installation)
    - [Configuration](#configuration)
  - [How to Use](#how-to-use)
    - [Activation per Player](#activation-per-player)
    - [Deactivation per Player](#deactivation-per-player)
    - [State Verification](#state-verification)
    - [Integration with Anti-Cheat](#integration-with-anti-cheat)
    - [Detection Logging](#detection-logging)
    - [Minigame Example](#minigame-example)
  - [Possible Applications](#possible-applications)
  - [Customization](#customization)
    - [Adjusting Defines](#adjusting-defines)
    - [Callback Customization](#callback-customization)
  - [Testing and Validation](#testing-and-validation)
  - [Limitations and Considerations](#limitations-and-considerations)
  - [License](#license)
    - [Conditions:](#conditions)

## Features

- **Comprehensive C-Bug Detection**: Identifies the following variants:
  - Classic
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Rapid shots
- **Per-Player Control**: Enables or disables detection individually for each player.
- **Ping Adjustment**: Adapts detection times based on the player's ping, minimizing false positives.
- **False Positive Protection**: Validates player state, weapon, ammunition, and animations to ensure accuracy.
- **Customizable Settings**: Defines detection parameters via constants (`defines`), although default values are optimized.
- **Extensible Callback**: The `CbugDetection_OnDetected` callback allows integration with custom systems.
- **Lightweight and Optimized**: Uses only `OnPlayerKeyStateChange` and `OnPlayerWeaponShot`, with minimal performance impact.

## How It Works

The include monitors player actions through the **OnPlayerKeyStateChange** and **OnPlayerWeaponShot** callbacks, using a **suspicion score** system (`cbug_d_suspicion_score`) to identify C-Bug patterns. When the score exceeds a configurable threshold (`CBUG_D_SUSPICION_THRESHOLD`), the `CbugDetection_OnDetected` callback is triggered.

> [!IMPORTANT]
> The system is designed to be robust, with rigorous validations that minimize **false positives**, **although rare cases may occur**, as detailed in the limitations section.

### Detection Logic
1. **Suspicion Score**:
   - Suspicious actions (e.g., crouching, weapon switching, rapid shots) increment `cbug_d_suspicion_score` with specific weights.
   - Example: Crouching after shooting adds **4.0** points; rapid shots add **3.0** points.
   - The score decays over time (`CBUG_D_SUSPICION_DECAY = 0.5` per second) to prevent accumulation of unrelated actions.

2. **Time Windows**:
   - Actions are considered suspicious if they occur shortly after a shot (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Rapid shots are detected if they occur within `CBUG_D_SHOT_INTERVAL = 200ms`.

3. **Validations**:
   - Monitors only specific weapons (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - Detection is disabled if:
     - The player is not on foot (`PLAYER_STATE_ONFOOT`).
     - The player is running or jumping (checked via animations).
     - There is no ammunition (`GetPlayerAmmo(playerid) <= 0`).
     - Detection is disabled for the player (`cbug_d_player_status`).

4. **Ping Adjustment**:
   - The `CbugDetection_GetPingAdjtThresh` function adjusts times based on ping:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - This ensures fairness for players with higher latency (e.g., **200**-**300ms**).

5. **Cooldown**:
   - An interval (`CBUG_D_COOLDOWN_TIME = 1500ms`) prevents repeated detections for the same sequence.

### Monitored Events
- **OnPlayerKeyStateChange**:
  - Tracks pressed keys (crouch, sprint, left/right) and weapon switches.
  - Detects patterns like crouching or rolling after shooting.
  - Example:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Additional checks for Rollbug, Quick C-switch, etc.
    }
    ```

- **OnPlayerWeaponShot**:
  - Monitors shots and rapid sequences.
  - Updates the score for consecutive rapid shots.
  - Example:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Initializes the player's detection state as disabled and resets variables:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Settings (Defines)
The defines control the detection behavior. Default values are optimized and recommended:

| Define                        | Default Value        | Description                                                                            |
|-------------------------------|----------------------|----------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Score required for detection. Higher values reduce sensitivity.                        |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Score decay per second.                                                               |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Window (**ms**) for actions after a shot to be considered suspicious.                  |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Window (**ms**) for detecting consecutive rapid shots.                                 |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Cooldown (**ms**) between detections to avoid repetition.                              |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Multiplier for ping adjustment in times.                                              |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Buffer (**ms**) for detecting weapon switch after a shot.                              |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Time (**ms**) after the last action to reset the score.                                |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | Monitored weapons (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**). |

> [!WARNING]
> It is not recommended to change the define values, as the defaults were carefully calibrated to balance accuracy and performance.

## Code Structure

The include is organized into distinct sections, each with a specific purpose:

1. **Header**:
   - Defines the include guard (`_cbug_detection_included`) and checks for the inclusion of `a_samp` or `open.mp`.

2. **Settings (Defines)**:
   - Defines constants for detection parameters (e.g., `CBUG_D_SUSPICION_THRESHOLD`).
   - Uses `#if defined` to avoid redefinition conflicts.

3. **Global Variables**:
   - Stores player states:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // State per player
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Suspicion score
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Last shot
     // ... other variables for tracking
     ```

4. **Utility Functions (Static Stock)**:
   - Internal functions for detection logic:
     - `CbugDetection_GetPingAdjtThresh`: Adjusts times by ping.
     - `CbugDetection_IsCbugWeapon`: Checks monitored weapons.
     - `CbugDetection_IsJumping`: Detects jump animations.
     - `CbugDetection_IsRunning`: Detects running animations.
     - `CbugDetection_ResetVariables`: Resets player variables.
   - Example:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... other variables to be reset...

         return true;
     }
     ```

5. **Public Functions (Stock)**:
   - Functions for system control:
     - `CbugDetection_Player`: Enables or disables detection for a specific player.
     - `CbugDetection_IsActive`: Checks if detection is active for a player.
   - Example:
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
   - Implements logic in `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect`, and `OnPlayerDisconnect`.
   - Provides `CbugDetection_OnDetected` for customization.
   - Example:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **ALS Hooks**:
   - Integrates standard SA-MP callbacks for compatibility:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... other ALS to be defined...
     ```

## Installation and Configuration

### Installation
1. **Download the Include**:
   - Place the `cbug-detection.inc` file in the `pawno/include` directory.

2. **Include in Gamemode**:
   - Add to the beginning of the gamemode:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> Ensure `a_samp` or `open.mp` is included before `cbug-detection.inc`, or the compiler will report errors.

3. **Compile the Gamemode**:
   - Use the **Pawn** compiler to compile the gamemode, checking for the absence of errors.

### Configuration
- **Define the Callback**:
  - Implement `CbugDetection_OnDetected` in the gamemode to handle detections:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "Player %d detected using C-Bug!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## How to Use

The include offers individual control for C-Bug detection per player. Below are practical usage examples:

### Activation per Player
- Enable for a specific player, e.g., in a combat event:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Deactivation per Player
- Disable for a specific player, e.g., for administrators:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### State Verification
- Check if detection is active for a player:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "Detection is active for you.");
  ```

### Integration with Anti-Cheat
- Apply punishments in `CbugDetection_OnDetected`:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Player %s (ID: %d) detected using C-Bug!", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Detection Logging
- Log detections to a file:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Player %s (ID: %d) detected using C-Bug\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Minigame Example
- Enable detection only for specific players in a minigame:
  ```pawn
  CMD:startminigame(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "C-Bug detection enabled for you in the minigame!");

      return 1;
  }

  CMD:endminigame(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "C-Bug detection disabled for you!");

      return 1;
  }
  ```

> [!TIP]
> Use `CbugDetection_IsActive` to check the detection state before performing conditional actions, such as alerts or punishments.

## Possible Applications

**Cbug Detection** is highly versatile and can be used in various contexts:

- **Roleplay (RP) Servers**: Enforce rules against **C-Bug** for realistic combat.
- **TDM/DM Servers**: Ensure balance in combat by detecting **C-Bug**.
- **Monitoring**: Analyze player behavior or identify frequent offenders.
- **Minigames and Events**: Control **C-Bug** only in specific events.
- **Custom Mechanics**: Integrate with scoring or penalty systems.

> [!NOTE]
> The ability to preserve individual settings when enabling or disabling globally makes the include ideal for servers with dynamic rules.

## Customization

Although the include is optimized with default settings, it is possible to adjust the defines or the `CbugDetection_OnDetected` callback.

> [!WARNING]
> Changing defines or other settings **is not recommended**, as the default values were tested and balanced for maximum effectiveness. Modifications may cause false positives, inaccurate detections, or even prevent proper functioning.

### Adjusting Defines
- **High-Ping Servers**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Stricter Detection**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Custom Weapons**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Callback Customization
- Add advanced logic:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Warning %d: Player %s (ID: %d) detected using C-Bug!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Repeated use of C-Bug");
      
      return 1;
  }
  ```

## Testing and Validation

To ensure proper functioning, perform the following tests:

- **C-Bug Detection**:
  - Test all listed variants with monitored weapons.
  - Confirm that `CbugDetection_OnDetected` is triggered after **2**-**3** sequences.

- **False Positives**:
  - Perform actions like running, jumping, or shooting without crouching.
  - Verify no detections occur for non-C-Bug-related actions.

- **Per-Player Control**:
  - Test `CbugDetection_Player` for enabling/disabling per player.
  - Confirm detection only occurs for players with `cbug_d_player_status` active.

- **State Verification**:
  - Use `CbugDetection_IsActive` to confirm the state per player.

- **OnPlayerConnect**:
  - Verify new players start with detection disabled (`cbug_d_player_status = false`).

- **High Ping**:
  - Simulate ping of **200**-**300ms** and verify detection accuracy.

- **Performance**:
  - Test with **50**+ players to confirm efficiency.

> [!TIP]
> Test in a controlled environment with few players before deploying to populated servers.

## Limitations and Considerations

- **Callback Dependency**:
  - Relies on `OnPlayerWeaponShot` and `OnPlayerKeyStateChange`, which may be affected by lag or other includes.
- **Extreme Ping**:
  - Very high latencies (>**500ms**) may require adjustments to `CBUG_D_PING_MULTIPLIER`.
- **Animations**:
  - Detection of running/jumping uses animation indices, which may vary in modified clients.
- **False Positives**:
  - Although robust, **false positives may occur in rare cases**, such as legitimate rapid actions, which is considered normal.

> [!IMPORTANT]
> **False positives** are minimized by validations, but they may occur in atypical scenarios. Monitor detections to adjust usage if necessary.

## License

This include is protected under the Apache License 2.0, which allows:

- ✔️ Commercial and private use
- ✔️ Source code modification
- ✔️ Code distribution
- ✔️ Patent grant

### Conditions:

- Maintain copyright notice
- Document significant changes
- Include Apache License 2.0 copy

For more details about the license: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - All rights reserved**