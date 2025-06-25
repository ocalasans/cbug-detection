# Cbug Detection

Le **Cbug Detection** est un include flexible pour **San Andreas Multiplayer (SA-MP)** conçu pour détecter l'**exploit C-Bug**, une technique permettant aux joueurs de tirer plus rapidement que prévu en exploitant les mécaniques du jeu. Cet **include** offre aux **développeurs de serveurs** un outil fiable pour identifier l'utilisation du **C-Bug**, permettant une intégration avec des systèmes comme l'anti-cheat, la surveillance du comportement ou des règles de jeu personnalisées.

Dans le contexte de **SA-MP**, le **C-Bug** est une pratique impliquant des actions rapides, comme s'accroupir ou changer d'arme, après avoir tiré avec certaines armes, réduisant ainsi le temps de rechargement et conférant un avantage injuste dans les combats. Cet **include** détecte ces actions de manière précise, étant utile dans les serveurs **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, les mini-jeux ou tout scénario nécessitant un contrôle strict des mécaniques de jeu. Sa polyvalence permet des utilisations au-delà de l'anti-cheat, comme l'analyse du gameplay ou la création de mécaniques uniques.

> [!NOTE]
> Le **Cbug Detection** ne se limite pas à bloquer le **C-Bug** ; il peut être intégré dans des systèmes de surveillance, des événements spécifiques ou des mini-jeux avec des règles personnalisées. **En résumé, ce sont les développeurs qui décident de l'action à entreprendre lorsqu'un joueur est détecté utilisant le C-Bug**, offrant ainsi une grande flexibilité dans l'application de la détection selon le contexte du serveur.

## Langues

- Português: [README](../../)
- Deutsch: [README](../Deutsch/README.md)
- English: [README](../English/README.md)
- Español: [README](../Espanol/README.md)
- Italiano: [README](../Italiano/README.md)
- Polski: [README](../Polski/README.md)
- Русский: [README](../Русский/README.md)
- Svenska: [README](../Svenska/README.md)
- Türkçe: [README](../Turkce/README.md)

## Table des matières

- [Cbug Detection](#cbug-detection)
  - [Langues](#langues)
  - [Table des matières](#table-des-matières)
  - [Fonctionnalités](#fonctionnalités)
  - [Comment ça fonctionne](#comment-ça-fonctionne)
    - [Logique de détection](#logique-de-détection)
    - [Événements surveillés](#événements-surveillés)
    - [Paramètres (Defines)](#paramètres-defines)
  - [Structure du code](#structure-du-code)
  - [Installation et configuration](#installation-et-configuration)
    - [Installation](#installation)
    - [Configuration](#configuration)
  - [Comment utiliser](#comment-utiliser)
    - [Activation par joueur](#activation-par-joueur)
    - [Désactivation par joueur](#désactivation-par-joueur)
    - [Vérification de l'état](#vérification-de-létat)
    - [Intégration avec l'anti-cheat](#intégration-avec-lanti-cheat)
    - [Enregistrement des détections](#enregistrement-des-détections)
    - [Exemple dans un mini-jeu](#exemple-dans-un-mini-jeu)
  - [Applications possibles](#applications-possibles)
  - [Personnalisation](#personnalisation)
    - [Ajustement des Defines](#ajustement-des-defines)
    - [Personnalisation du Callback](#personnalisation-du-callback)
  - [Tests et validation](#tests-et-validation)
  - [Limites et considérations](#limites-et-considérations)
  - [Licence](#licence)
    - [Conditions](#conditions)

## Fonctionnalités

- **Détection complète du C-Bug**: Identifie les variantes suivantes:
  - Classic
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Disparos rápidos
- **Contrôle par joueur**: Active ou désactive la détection individuellement pour chaque joueur.
- **Ajustement en fonction du ping**: Adapte les temps de détection en fonction du ping du joueur, minimisant les faux positifs.
- **Protection contre les faux positifs**: Valide l'état du joueur, l'arme, les munitions et les animations pour garantir la précision.
- **Paramètres personnalisables**: Définit les paramètres de détection via des constantes (`defines`), bien que les valeurs par défaut soient optimisées.
- **Callback extensible**: Le callback `CbugDetection_OnDetected` permet une intégration avec des systèmes personnalisés.
- **Léger et optimisé**: Utilise uniquement `OnPlayerKeyStateChange` et `OnPlayerWeaponShot`, avec un impact minimal sur les performances.

## Comment ça fonctionne

L'include surveille les actions des joueurs via les callbacks **OnPlayerKeyStateChange** et **OnPlayerWeaponShot**, en utilisant un système de **score de suspicion** (`cbug_d_suspicion_score`) pour identifier les schémas de C-Bug. Lorsque le score dépasse un seuil configurable (`CBUG_D_SUSPICION_THRESHOLD`), le callback `CbugDetection_OnDetected` est déclenché.

> [!IMPORTANT]
> Le système est conçu pour être robuste, avec des validations strictes minimisant les **faux positifs**, **bien que des cas rares puissent survenir**, comme détaillé dans la section des limites.

### Logique de détection
1. **Score de suspicion**:
   - Les actions suspectes (par exemple, s'accroupir, changer d'arme, tirs rapides) incrémentent `cbug_d_suspicion_score` avec des poids spécifiques.
   - Exemple: S'accroupir après un tir ajoute **4.0** points ; les tirs rapides ajoutent **3.0** points.
   - Le score diminue avec le temps (`CBUG_D_SUSPICION_DECAY = 0.5` par seconde) pour éviter l'accumulation d'actions non liées.

2. **Fenêtres temporelles**:
   - Les actions sont considérées comme suspectes si elles surviennent juste après un tir (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Les tirs rapides sont détectés s'ils surviennent dans un délai de `CBUG_D_SHOT_INTERVAL = 200ms`.

3. **Validations**:
   - Surveille uniquement les armes spécifiques (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - La détection est désactivée si:
     - Le joueur n'est pas à pied (`PLAYER_STATE_ONFOOT`).
     - Le joueur court ou saute (vérifié par les animations).
     - Il n'y a pas de munitions (`GetPlayerAmmo(playerid) <= 0`).
     - La détection est désactivée pour le joueur (`cbug_d_player_status`).

4. **Ajustement en fonction du ping**:
   - La fonction `CbugDetection_GetPingAdjtThresh` ajuste les temps en fonction du ping:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Cela garantit l'équité pour les joueurs avec une latence plus élevée (par exemple, **200**-**300ms**).

5. **Temps de récupération**:
   - Un intervalle (`CBUG_D_COOLDOWN_TIME = 1500ms`) évite les détections répétées pour la même séquence.

### Événements surveillés
- **OnPlayerKeyStateChange**:
  - Suit les touches pressées (s'accroupir, courir, gauche/droite) et les changements d'arme.
  - Détecte des schémas comme s'accroupir ou rouler après un tir.
  - Exemple:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Vérifications supplémentaires pour Rollbug, Quick C-switch, etc.
    }
    ```

- **OnPlayerWeaponShot**:
  - Surveille les tirs et les séquences rapides.
  - Met à jour le score pour les tirs consécutifs rapides.
  - Exemple:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Initialise l'état de détection du joueur comme désactivé et réinitialise les variables:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Paramètres (Defines)
Les defines contrôlent le comportement de la détection. Les valeurs par défaut sont optimisées et recommandées:

| Define                        | Valeur par défaut    | Description                                                                            |
|-------------------------------|----------------------|----------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Score requis pour la détection. Des valeurs plus élevées réduisent la sensibilité.     |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Diminution du score par seconde.                                                       |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Fenêtre (**ms**) pour que les actions après un tir soient considérées suspectes.       |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Fenêtre (**ms**) pour détecter des tirs rapides consécutifs.                           |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Temps de récupération (**ms**) entre les détections pour éviter les répétitions.       |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Multiplicateur pour l'ajustement du ping dans les temps.                               |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Tampon (**ms**) pour la détection du changement d'arme après un tir.                   |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Temps (**ms**) après la dernière action pour réinitialiser le score.                   |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | Armes surveillées (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**). |

> [!WARNING]
> Il n'est pas recommandé de modifier les valeurs des defines, car les valeurs par défaut ont été soigneusement calibrées pour équilibrer précision et performance.

## Structure du code

L'include est organisé en sections distinctes, chacune ayant une fonction spécifique:

1. **En-tête**:
   - Définit le garde d'inclusion (`_cbug_detection_included`) et vérifie l'inclusion de `a_samp` ou `open.mp`.

2. **Paramètres (Defines)**:
   - Définit des constantes pour les paramètres de détection (par exemple, `CBUG_D_SUSPICION_THRESHOLD`).
   - Utilise `#if defined` pour éviter les conflits de redéfinition.

3. **Variables globales**:
   - Stocke les états des joueurs:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // État par joueur
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Score de suspicion
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Dernier tir
     // ... autres variables pour le suivi
     ```

4. **Fonctions utilitaires (Static Stock)**:
   - Fonctions internes pour la logique de détection:
     - `CbugDetection_GetPingAdjtThresh`: Ajuste les temps en fonction du ping.
     - `CbugDetection_IsCbugWeapon`: Vérifie les armes surveillées.
     - `CbugDetection_IsJumping`: Détecte les animations de saut.
     - `CbugDetection_IsRunning`: Détecte les animations de course.
     - `CbugDetection_ResetVariables`: Réinitialise les variables du joueur.
   - Exemple:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... autres variables à réinitialiser...

         return true;
     }
     ```

5. **Fonctions publiques (Stock)**:
   - Fonctions pour contrôler le système:
     - `CbugDetection_Player`: Active ou désactive la détection pour un joueur spécifique.
     - `CbugDetection_IsActive`: Vérifie si la détection est active pour un joueur.
   - Exemple:
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
   - Implémente la logique dans `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect`, et `OnPlayerDisconnect`.
   - Fournit `CbugDetection_OnDetected` pour la personnalisation.
   - Exemple:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **Hooks ALS**:
   - Intègre les callbacks standard de SA-MP pour la compatibilité:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... autres ALS à définir...
     ```

## Installation et configuration

### Installation
1. **Téléchargez l'include**:
 - Placez le fichier `cbug-detection.inc` dans le répertoire `pawno/include`.

2. **Incluez dans le gamemode**:
 - Ajoutez au début du gamemode:
   ```pawn
   #include <a_samp>
   #include <cbug-detection>
   ```

> [!WARNING]
> Assurez-vous d'inclure `a_samp` ou `open.mp` avant `cbug-detection.inc`, sinon le compilateur signalera des erreurs.

3. **Compilez le gamemode**:
 - Utilisez le compilateur **Pawn** pour compiler le gamemode, en vérifiant l'absence d'erreurs.

### Configuration
- **Définissez le callback**:
  - Implémentez `CbugDetection_OnDetected` dans le gamemode pour gérer les détections:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "Joueur %d détecté utilisant C-Bug !", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Comment utiliser

L'include offre un contrôle individuel pour la détection de C-Bug par joueur. Voici des exemples pratiques d'utilisation:

### Activation par joueur
- Activez pour un joueur spécifique, par exemple, dans un événement de combat:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Désactivation par joueur
- Désactivez pour un joueur spécifique, par exemple, pour les administrateurs:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Vérification de l'état
- Vérifiez si la détection est active pour un joueur:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "La détection est active pour vous.");
  ```

### Intégration avec l'anti-cheat
- Appliquez des sanctions dans `CbugDetection_OnDetected`:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Joueur %s (ID: %d) détecté utilisant C-Bug !", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Enregistrement des détections
- Enregistrez les détections dans un fichier:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Joueur %s (ID: %d) détecté utilisant C-Bug\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Exemple dans un mini-jeu
- Activez la détection uniquement pour des joueurs spécifiques dans un mini-jeu:
  ```pawn
  CMD:iniciarminigame(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "Détection de C-Bug activée pour vous dans le mini-jeu !");

      return 1;
  }

  CMD:finalizarminigame(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "Détection de C-Bug désactivée pour vous !");

      return 1;
  }
  ```

> [!TIP]
> Utilisez `CbugDetection_IsActive` pour vérifier l'état de la détection avant d'exécuter des actions conditionnelles, comme des alertes ou des sanctions.

## Applications possibles

Le **Cbug Detection** est hautement polyvalent et peut être utilisé dans divers contextes:

- **Serveurs Roleplay (RP)**: Renforcez les règles contre le **C-Bug** pour des combats réalistes.
- **Serveurs TDM/DM**: Assurez l'équilibre dans les combats en détectant le **C-Bug**.
- **Surveillance**: Analysez le comportement des joueurs ou identifiez les contrevenants fréquents.
- **Mini-jeux et événements**: Contrôlez le **C-Bug** uniquement dans des événements spécifiques.
- **Mécaniques personnalisées**: Intégrez avec des systèmes de score ou de pénalités.

> [!NOTE]
> La capacité à préserver les configurations individuelles lors de l'activation ou de la désactivation globale rend l'include idéal pour les serveurs avec des règles dynamiques.

## Personnalisation

Bien que l'include soit optimisé avec des configurations par défaut, il est possible d'ajuster les defines ou le callback `CbugDetection_OnDetected`.

> [!WARNING]
> Modifier les defines ou autres configurations **n'est pas recommandé**, car les valeurs par défaut ont été testées et équilibrées pour une efficacité maximale. Les modifications peuvent provoquer des faux positifs, des détections imprécises ou même empêcher le fonctionnement correct.

### Ajustement des Defines
- **Serveurs avec un ping élevé**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Détection plus stricte**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Armes personnalisées**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Personnalisation du Callback
- Ajoutez une logique avancée:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Avertissement %d: Joueur %s (ID: %d) détecté utilisant C-Bug !", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Utilisation répétée de C-Bug");
      
      return 1;
  }
  ```

## Tests et validation

Pour garantir un fonctionnement correct, effectuez les tests suivants:

- **Détection de C-Bug**:
  - Testez toutes les variantes listées avec les armes surveillées.
  - Confirmez que `CbugDetection_OnDetected` est déclenché après **2**-**3** séquences.

- **Faux positifs**:
  - Effectuez des actions comme courir, sauter ou tirer sans s'accroupir.
  - Vérifiez qu'aucune détection n'a lieu pour des actions non liées au **C-Bug**.

- **Contrôle par joueur**:
  - Testez `CbugDetection_Player` pour l'activation/désactivation par joueur.
  - Confirmez que la détection ne se produit que pour les joueurs avec `cbug_d_player_status` actif.

- **Vérification de l'état**:
  - Utilisez `CbugDetection_IsActive` pour confirmer l'état par joueur.

- **OnPlayerConnect**:
  - Vérifiez que les nouveaux joueurs commencent avec la détection désactivée (`cbug_d_player_status = false`).

- **Ping élevé**:
  - Simulez un ping de **200**-**300ms** et vérifiez la précision de la détection.

- **Performance**:
  - Testez avec **50**+ joueurs pour confirmer l'efficacité.

> [!TIP]
> Testez dans un environnement contrôlé avec peu de joueurs avant de déployer sur des serveurs très fréquentés.

## Limites et considérations

- **Dépendance aux callbacks**:
  - Dépend de `OnPlayerWeaponShot` et `OnPlayerKeyStateChange`, qui peuvent être affectés par le lag ou d'autres includes.
- **Ping extrême**:
  - Des latences très élevées (>**500ms**) peuvent nécessiter des ajustements dans `CBUG_D_PING_MULTIPLIER`.
- **Animations**:
  - La détection de course/saut utilise des indices d'animation, qui peuvent varier sur des clients modifiés.
- **Faux positifs**:
  - Bien que robuste, **des faux positifs peuvent survenir dans des cas rares**, comme des actions légitimes en séquence rapide, ce qui est considéré comme normal.

> [!IMPORTANT]
> Les **faux positifs** sont minimisés par les validations, mais peuvent survenir dans des scénarios atypiques. Surveillez les détections pour ajuster l'utilisation, si nécessaire.

## Licence

Cet include est protégé sous la Licence Apache 2.0, qui permet:

- ✔️ Utilisation commerciale et privée
- ✔️ Modification du code source
- ✔️ Distribution du code
- ✔️ Octroi de brevets

### Conditions

- Conserver l'avis de droit d'auteur
- Documenter les modifications significatives
- Inclure une copie de la licence Apache 2.0

Pour plus de détails sur la licence: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - Tous droits réservés**
