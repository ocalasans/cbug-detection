/*
 * Cbug Detection - include for SA-MP (San Andreas Multiplayer)
 * Copyright (c) Calasans
 * GitHub: https://github.com/ocalasans
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#if defined _cbug_detection
    #undef _cbug_detection
#endif

#if defined _cbug_detection_included
    #endinput
#endif
#define _cbug_detection_included

#if !defined _INC_open_mp
    #if !defined _samp_included
        #error "The "a_samp" ("or open.mp") library has not been activated. Please activate it at the beginning of your Gamemode, Ex: "#include <a_samp>" ("or <open.mp>")"
    #endif
#endif

/*
$$$$$$$\             $$$$$$\  $$\           $$\   $$\     $$\                               
$$  __$$\           $$  __$$\ \__|          \__|  $$ |    \__|                              
$$ |  $$ | $$$$$$\  $$ /  \__|$$\ $$$$$$$\  $$\ $$$$$$\   $$\  $$$$$$\  $$$$$$$\   $$$$$$$\ 
$$ |  $$ |$$  __$$\ $$$$\     $$ |$$  __$$\ $$ |\_$$  _|  $$ |$$  __$$\ $$  __$$\ $$  _____|
$$ |  $$ |$$$$$$$$ |$$  _|    $$ |$$ |  $$ |$$ |  $$ |    $$ |$$ /  $$ |$$ |  $$ |\$$$$$$\  
$$ |  $$ |$$   ____|$$ |      $$ |$$ |  $$ |$$ |  $$ |$$\ $$ |$$ |  $$ |$$ |  $$ | \____$$\ 
$$$$$$$  |\$$$$$$$\ $$ |      $$ |$$ |  $$ |$$ |  \$$$$  |$$ |\$$$$$$  |$$ |  $$ |$$$$$$$  |
\_______/  \_______|\__|      \__|\__|  \__|\__|   \____/ \__| \______/ \__|  \__|\_______/ 
*/

#if defined CBUG_D_SUSPICION_THRESHOLD
    #undef CBUG_D_SUSPICION_THRESHOLD
#endif
#define CBUG_D_SUSPICION_THRESHOLD 10.0

#if defined CBUG_D_SUSPICION_DECAY
    #undef CBUG_D_SUSPICION_DECAY
#endif
#define CBUG_D_SUSPICION_DECAY 0.5

#if defined CBUG_D_SEQUENCE_INTERVAL
    #undef CBUG_D_SEQUENCE_INTERVAL
#endif
#define CBUG_D_SEQUENCE_INTERVAL 1500

#if defined CBUG_D_SHOT_INTERVAL
    #undef CBUG_D_SHOT_INTERVAL
#endif
#define CBUG_D_SHOT_INTERVAL 200

#if defined CBUG_D_COOLDOWN_TIME
    #undef CBUG_D_COOLDOWN_TIME
#endif
#define CBUG_D_COOLDOWN_TIME 1500

#if defined CBUG_D_PING_MULTIPLIER
    #undef CBUG_D_PING_MULTIPLIER
#endif
#define CBUG_D_PING_MULTIPLIER 0.01

#if defined CBUG_D_WEAPON_BUFFER_TIME
    #undef CBUG_D_WEAPON_BUFFER_TIME
#endif
#define CBUG_D_WEAPON_BUFFER_TIME 500

#if defined CBUG_D_SCORE_RESET_TIME
    #undef CBUG_D_SCORE_RESET_TIME
#endif
#define CBUG_D_SCORE_RESET_TIME 2000

#if defined CBUG_D_VALID_WEAPONS
    #undef CBUG_D_VALID_WEAPONS
#endif
#define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}

/*
$$\    $$\                    $$\           $$\       $$\                     
$$ |   $$ |                   \__|          $$ |      $$ |                    
$$ |   $$ |$$$$$$\   $$$$$$\  $$\  $$$$$$\  $$$$$$$\  $$ | $$$$$$\   $$$$$$$\ 
\$$\  $$  |\____$$\ $$  __$$\ $$ | \____$$\ $$  __$$\ $$ |$$  __$$\ $$  _____|
 \$$\$$  / $$$$$$$ |$$ |  \__|$$ | $$$$$$$ |$$ |  $$ |$$ |$$$$$$$$ |\$$$$$$\  
  \$$$  / $$  __$$ |$$ |      $$ |$$  __$$ |$$ |  $$ |$$ |$$   ____| \____$$\ 
   \$  /  \$$$$$$$ |$$ |      $$ |\$$$$$$$ |$$$$$$$  |$$ |\$$$$$$$\ $$$$$$$  |
    \_/    \_______|\__|      \__| \_______|\_______/ \__| \_______|\_______/ 
*/

static bool:cbug_d_player_status[MAX_PLAYERS];
static Float:cbug_d_suspicion_score[MAX_PLAYERS];
static cbug_d_last_fire_time[MAX_PLAYERS];
static cbug_d_last_roll_time[MAX_PLAYERS];
static cbug_d_last_weapon_switch_time[MAX_PLAYERS];
static cbug_d_last_detection_time[MAX_PLAYERS];
static cbug_d_last_action_time[MAX_PLAYERS];
static cbug_d_last_weapon[MAX_PLAYERS];
static cbug_d_original_weapon[MAX_PLAYERS];
static bool:cbug_d_weapon_switched[MAX_PLAYERS];
static cbug_d_last_shot_time[MAX_PLAYERS];
static cbug_d_shot_count[MAX_PLAYERS];

/*
$$$$$$$$\                                                          $$\           
$$  _____|                                                         $$ |          
$$ |    $$$$$$\   $$$$$$\  $$\  $$\  $$\  $$$$$$\   $$$$$$\   $$$$$$$ | $$$$$$$\ 
$$$$$\ $$  __$$\ $$  __$$\ $$ | $$ | $$ | \____$$\ $$  __$$\ $$  __$$ |$$  _____|
$$  __|$$ /  $$ |$$ |  \__|$$ | $$ | $$ | $$$$$$$ |$$ |  \__|$$ /  $$ |\$$$$$$\  
$$ |   $$ |  $$ |$$ |      $$ | $$ | $$ |$$  __$$ |$$ |      $$ |  $$ | \____$$\ 
$$ |   \$$$$$$  |$$ |      \$$$$$\$$$$  |\$$$$$$$ |$$ |      \$$$$$$$ |$$$$$$$  |
\__|    \______/ \__|       \_____\____/  \_______|\__|       \_______|\_______/ 
*/

forward CbugDetection_OnDetected(playerid);

/*
$$$$$$$$\                              $$\     $$\                               
$$  _____|                             $$ |    \__|                              
$$ |   $$\   $$\ $$$$$$$\   $$$$$$$\ $$$$$$\   $$\  $$$$$$\  $$$$$$$\   $$$$$$$\ 
$$$$$\ $$ |  $$ |$$  __$$\ $$  _____|\_$$  _|  $$ |$$  __$$\ $$  __$$\ $$  _____|
$$  __|$$ |  $$ |$$ |  $$ |$$ /        $$ |    $$ |$$ /  $$ |$$ |  $$ |\$$$$$$\  
$$ |   $$ |  $$ |$$ |  $$ |$$ |        $$ |$$\ $$ |$$ |  $$ |$$ |  $$ | \____$$\ 
$$ |   \$$$$$$  |$$ |  $$ |\$$$$$$$\   \$$$$  |$$ |\$$$$$$  |$$ |  $$ |$$$$$$$  |
\__|    \______/ \__|  \__| \_______|   \____/ \__| \______/ \__|  \__|\_______/ 
*/

// They cannot be used by users, only by the include itself, because of this they are static! ;)
static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
    return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);

static stock bool:CbugDetection_IsCbugWeapon(cbug_d_weaponid) {
    new cbug_d_valid_weapons[] = CBUG_D_VALID_WEAPONS;

    for (new cbug_d_i = 0; cbug_d_i < sizeof(cbug_d_valid_weapons); cbug_d_i++) {
        if (cbug_d_weaponid == cbug_d_valid_weapons[cbug_d_i])
            return true;
    }

    return false;
}

static stock bool:CbugDetection_IsJumping(playerid) {
    new cbug_d_anim = GetPlayerAnimationIndex(playerid);
    return (cbug_d_anim == 1195 || cbug_d_anim == 1196 || cbug_d_anim == 1197 || cbug_d_anim == 1198);
}

static stock bool:CbugDetection_IsRunning(playerid) {
    new cbug_d_anim = GetPlayerAnimationIndex(playerid);
    return (cbug_d_anim == 1231 || cbug_d_anim == 1266 || cbug_d_anim == 1223);
}

static stock bool:CbugDetection_ResetVariables(playerid) {
    if (cbug_d_suspicion_score[playerid] != 0.0)
        cbug_d_suspicion_score[playerid] = 0.0;

    if (cbug_d_last_fire_time[playerid] != 0)
        cbug_d_last_fire_time[playerid] = 0;

    if (cbug_d_last_roll_time[playerid] != 0)
        cbug_d_last_roll_time[playerid] = 0;

    if (cbug_d_last_weapon_switch_time[playerid] != 0)
        cbug_d_last_weapon_switch_time[playerid] = 0;

    if (cbug_d_last_detection_time[playerid] != 0)
        cbug_d_last_detection_time[playerid] = 0;

    if (cbug_d_last_action_time[playerid] != 0)
        cbug_d_last_action_time[playerid] = 0;

    if (cbug_d_last_weapon[playerid] != 0)
        cbug_d_last_weapon[playerid] = 0;

    if (cbug_d_original_weapon[playerid] != 0)
        cbug_d_original_weapon[playerid] = 0;

    if (cbug_d_weapon_switched[playerid] != false)
        cbug_d_weapon_switched[playerid] = false;

    if (cbug_d_last_shot_time[playerid] != 0)
        cbug_d_last_shot_time[playerid] = 0;

    if (cbug_d_shot_count[playerid] != 0)
        cbug_d_shot_count[playerid] = 0;

    return true;
}

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

stock bool:CbugDetection_IsActive(playerid) {
    if (!IsPlayerConnected(playerid) || playerid == INVALID_PLAYER_ID)
        return false;

    return cbug_d_player_status[playerid];
}

/*
 $$$$$$\            $$\ $$\ $$\                           $$\                 
$$  __$$\           $$ |$$ |$$ |                          $$ |                
$$ /  \__| $$$$$$\  $$ |$$ |$$$$$$$\   $$$$$$\   $$$$$$$\ $$ |  $$\  $$$$$$$\ 
$$ |       \____$$\ $$ |$$ |$$  __$$\  \____$$\ $$  _____|$$ | $$  |$$  _____|
$$ |       $$$$$$$ |$$ |$$ |$$ |  $$ | $$$$$$$ |$$ /      $$$$$$  / \$$$$$$\  
$$ |  $$\ $$  __$$ |$$ |$$ |$$ |  $$ |$$  __$$ |$$ |      $$  _$$<   \____$$\ 
\$$$$$$  |\$$$$$$$ |$$ |$$ |$$$$$$$  |\$$$$$$$ |\$$$$$$$\ $$ | \$$\ $$$$$$$  |
 \______/  \_______|\__|\__|\_______/  \_______| \_______|\__|  \__|\_______/ 
*/

public OnGameModeInit() {
    print(" ");
    print("__________________________________________________________________");
    print("||==============================================================||");
    print("||                                                              ||");
    print("||                        Cbug Detection                        ||");
    print("||                                                              ||");
    print("||               ,ad8888ba,           88888888ba,               ||");
    print("||              d8''    `'8b          88      `'8b              ||");
    print("||             d8'                    88        `8b             ||");
    print("||             88           aaaaaaaa  88         88             ||");
    print("||             88           ========  88         88             ||");
    print("||             Y8,                    88         8P             ||");
    print("||              Y8a.    .a8P          88      .a8P              ||");
    print("||               `'Y8888Y''           88888888Y''               ||");
    print("||                                                              ||");
    print("||                                                              ||");
    print("|| Developer: Calasans                                          ||");
    print("|| GitHub: https://github.com/ocalasans                         ||");
    print("|| Instagram: https://instagram.com/ocalasans                   ||");
    print("|| Discord: ocalasans                                           ||");
    print("||                                                              ||");
    print("|| [Info]: Include version: 1.0.0                               ||");
    print("||                                                              ||");
    print("||==============================================================||");
    print("__________________________________________________________________");
    print(" ");

    #if defined CBUG_D_OnGameModeInit
        return CBUG_D_OnGameModeInit();
    #else
        return 1;
    #endif
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if (!IsPlayerConnected(playerid) || !cbug_d_player_status[playerid])
        return 0;

    new cbug_d_current_time = GetTickCount();
    new cbug_d_weaponid = GetPlayerWeapon(playerid);
    new cbug_d_player_state = GetPlayerState(playerid);
    //
    new bool:cbug_d_is_valid_weapon = CbugDetection_IsCbugWeapon(cbug_d_weaponid) || (CbugDetection_IsCbugWeapon(cbug_d_last_weapon[playerid]) && 
        (cbug_d_current_time - cbug_d_last_weapon_switch_time[playerid] < CBUG_D_WEAPON_BUFFER_TIME));
    
    if (cbug_d_player_state != PLAYER_STATE_ONFOOT || !cbug_d_is_valid_weapon || CbugDetection_IsRunning(playerid) || CbugDetection_IsJumping(playerid)) {
        cbug_d_suspicion_score[playerid] = 0.0;
        cbug_d_shot_count[playerid] = 0;
        cbug_d_weapon_switched[playerid] = false;

        return 0;
    }

    if (cbug_d_last_action_time[playerid] != 0 && (cbug_d_current_time - cbug_d_last_action_time[playerid] > CBUG_D_SCORE_RESET_TIME)) {
        cbug_d_suspicion_score[playerid] = 0.0;
        cbug_d_last_fire_time[playerid] = 0;
        cbug_d_last_roll_time[playerid] = 0;
        cbug_d_last_weapon_switch_time[playerid] = 0;
        cbug_d_weapon_switched[playerid] = false;
    }

    if (cbug_d_last_fire_time[playerid] != 0) {
        new cbug_d_elapsed = cbug_d_current_time - cbug_d_last_fire_time[playerid];
        cbug_d_suspicion_score[playerid] -= (float(cbug_d_elapsed) / 1000.0) * CBUG_D_SUSPICION_DECAY;
        
        if (cbug_d_suspicion_score[playerid] < 0.0)
            cbug_d_suspicion_score[playerid] = 0.0;
    }

    if (cbug_d_weaponid != cbug_d_last_weapon[playerid]) {
        cbug_d_last_weapon[playerid] = cbug_d_weaponid;
        cbug_d_last_weapon_switch_time[playerid] = cbug_d_current_time;
        
        new cbug_d_time_since_last_fire = cbug_d_current_time - cbug_d_last_fire_time[playerid];
        new cbug_d_adjusted_threshold = floatround(CbugDetection_GetPingAdjtThresh(playerid, CBUG_D_SEQUENCE_INTERVAL));

        if (cbug_d_time_since_last_fire <= cbug_d_adjusted_threshold && cbug_d_last_fire_time[playerid] != 0) {
            cbug_d_weapon_switched[playerid] = true;
            cbug_d_suspicion_score[playerid] += 3.0;
            cbug_d_last_action_time[playerid] = cbug_d_current_time;
        }
        else if (cbug_d_weapon_switched[playerid] && cbug_d_weaponid == cbug_d_original_weapon[playerid] && cbug_d_time_since_last_fire <= cbug_d_adjusted_threshold) {
            cbug_d_suspicion_score[playerid] += 3.5;
            cbug_d_weapon_switched[playerid] = false;
            cbug_d_last_action_time[playerid] = cbug_d_current_time;
        }
    }

    if ((newkeys & (KEY_LEFT | KEY_RIGHT)) && !(oldkeys & (KEY_LEFT | KEY_RIGHT)))
        cbug_d_last_roll_time[playerid] = cbug_d_current_time;

    new Float:cbug_d_score_increment = 0.0;
    new cbug_d_time_since_last_fire = cbug_d_current_time - cbug_d_last_fire_time[playerid];
    new cbug_d_time_since_last_roll = cbug_d_current_time - cbug_d_last_roll_time[playerid];
    new cbug_d_time_since_last_sw = cbug_d_current_time - cbug_d_last_weapon_switch_time[playerid];
    new cbug_d_adjusted_threshold = floatround(CbugDetection_GetPingAdjtThresh(playerid, CBUG_D_SEQUENCE_INTERVAL));

    if (cbug_d_time_since_last_fire <= cbug_d_adjusted_threshold && cbug_d_last_fire_time[playerid] != 0) {
        if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
            cbug_d_score_increment = 4.0;
            
            if (cbug_d_time_since_last_roll <= cbug_d_adjusted_threshold && cbug_d_last_roll_time[playerid] != 0)
                cbug_d_score_increment = (cbug_d_score_increment > 4.5) ? cbug_d_score_increment : 4.5;
            
            if (cbug_d_weapon_switched[playerid] && cbug_d_time_since_last_sw <= cbug_d_adjusted_threshold) {
                cbug_d_score_increment = (cbug_d_score_increment > 4.5) ? cbug_d_score_increment : 4.5;
                cbug_d_weapon_switched[playerid] = false;
            }

            if (CbugDetection_IsJumping(playerid))
                cbug_d_score_increment = (cbug_d_score_increment > 3.5) ? cbug_d_score_increment : 3.5;

            if (newkeys & KEY_SPRINT)
                cbug_d_score_increment = (cbug_d_score_increment > 3.0) ? cbug_d_score_increment : 3.0;

            cbug_d_last_action_time[playerid] = cbug_d_current_time;
        }
    }

    if ((newkeys & (KEY_SPRINT | KEY_CROUCH)) == (KEY_SPRINT | KEY_CROUCH) && cbug_d_last_fire_time[playerid] != 0 && cbug_d_time_since_last_fire <= cbug_d_adjusted_threshold) {
        cbug_d_score_increment = (cbug_d_score_increment > 2.0) ? cbug_d_score_increment : 2.0;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }

    if (cbug_d_score_increment > 0.0) {
        cbug_d_suspicion_score[playerid] += cbug_d_score_increment;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }

    if (cbug_d_suspicion_score[playerid] >= CBUG_D_SUSPICION_THRESHOLD && (cbug_d_current_time - cbug_d_last_detection_time[playerid] > CBUG_D_COOLDOWN_TIME)) {
        CbugDetection_OnDetected(playerid);

        cbug_d_suspicion_score[playerid] = 0.0;
        cbug_d_shot_count[playerid] = 0;
        cbug_d_weapon_switched[playerid] = false;
        cbug_d_last_fire_time[playerid] = 0;
        cbug_d_last_action_time[playerid] = 0;
        cbug_d_last_detection_time[playerid] = cbug_d_current_time;
    }

    #if defined CBUG_D_OnPlayerKeyStateChange
        return CBUG_D_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
    #else
        return 1;
    #endif
}

public OnPlayerConnect(playerid) {
    CbugDetection_ResetVariables(playerid);

    #if defined CBUG_D_OnPlayerConnect
        return CBUG_D_OnPlayerConnect(playerid);
    #else
        return 1;
    #endif
}

public OnPlayerDisconnect(playerid, reason) {
    if (cbug_d_player_status[playerid])
        cbug_d_player_status[playerid] = false;

    CbugDetection_ResetVariables(playerid);

    #if defined CBUG_D_OnPlayerDisconnect
        return CBUG_D_OnPlayerDisconnect(playerid, reason);
    #else
        return 1;
    #endif
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) {
    if (!cbug_d_player_status[playerid] || !CbugDetection_IsCbugWeapon(weaponid) || GetPlayerAmmo(playerid) <= 0)
        return 0;

    new cbug_d_current_time = GetTickCount();

    cbug_d_last_fire_time[playerid] = cbug_d_current_time;
    cbug_d_original_weapon[playerid] = weaponid;
    cbug_d_weapon_switched[playerid] = false;
    cbug_d_last_action_time[playerid] = cbug_d_current_time;

    if (cbug_d_last_shot_time[playerid] != 0) {
        new cbug_d_time_since_last_shot = cbug_d_current_time - cbug_d_last_shot_time[playerid];
        new cbug_d_adjusted_shot_threshold = floatround(CbugDetection_GetPingAdjtThresh(playerid, CBUG_D_SHOT_INTERVAL));

        if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
            cbug_d_suspicion_score[playerid] += 3.0;
            cbug_d_shot_count[playerid]++;
            cbug_d_last_action_time[playerid] = cbug_d_current_time;
        }
        else
            cbug_d_shot_count[playerid] = 1;
    }
    else
        cbug_d_shot_count[playerid] = 1;

    cbug_d_last_shot_time[playerid] = cbug_d_current_time;

    if (cbug_d_suspicion_score[playerid] >= CBUG_D_SUSPICION_THRESHOLD && (cbug_d_current_time - cbug_d_last_detection_time[playerid] > CBUG_D_COOLDOWN_TIME)) {
        CbugDetection_OnDetected(playerid);

        cbug_d_suspicion_score[playerid] = 0.0;
        cbug_d_shot_count[playerid] = 0;
        cbug_d_weapon_switched[playerid] = false;
        cbug_d_last_fire_time[playerid] = 0;
        cbug_d_last_action_time[playerid] = 0;
        cbug_d_last_detection_time[playerid] = cbug_d_current_time;
    }

    #if defined CBUG_D_OnPlayerWeaponShot
        return CBUG_D_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, fX, fY, fZ);
    #else
        return 1;
    #endif
}

/*
 $$$$$$\  $$\       $$$$$$\  
$$  __$$\ $$ |     $$  __$$\ 
$$ /  $$ |$$ |     $$ /  \__|
$$$$$$$$ |$$ |     \$$$$$$\  
$$  __$$ |$$ |      \____$$\ 
$$ |  $$ |$$ |     $$\   $$ |
$$ |  $$ |$$$$$$$$\\$$$$$$  |
\__|  \__|\________|\______/ 
*/

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit CBUG_D_OnGameModeInit
#if defined CBUG_D_OnGameModeInit
    forward CBUG_D_OnGameModeInit();
#endif

#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect CBUG_D_OnPlayerConnect
#if !defined CBUG_D_OnPlayerConnect
    forward CBUG_D_OnPlayerConnect(playerid);
#endif

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect CBUG_D_OnPlayerDisconnect
#if !defined CBUG_D_OnPlayerDisconnect
    forward CBUG_D_OnPlayerDisconnect(playerid, reason);
#endif

#if defined _ALS_OnPlayerWeaponShot
    #undef OnPlayerWeaponShot
#else
    #define _ALS_OnPlayerWeaponShot
#endif
#define OnPlayerWeaponShot CBUG_D_OnPlayerWeaponShot
#if !defined CBUG_D_OnPlayerWeaponShot
    forward CBUG_D_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange CBUG_D_OnPlayerKeyStateChange
#if !defined CBUG_D_OnPlayerKeyStateChange
    forward CBUG_D_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif