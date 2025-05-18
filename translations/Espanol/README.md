# Cbug Detection

**Cbug Detection** es un include flexible para **San Andreas Multiplayer (SA-MP)** diseñado para detectar el **exploit C-Bug**, una técnica que permite a los jugadores disparar más rápido de lo previsto al explotar mecánicas del juego. Este **include** ofrece a los **desarrolladores de servidores** una herramienta confiable para identificar el uso de **C-Bug**, posibilitando la integración con sistemas como anti-cheat, monitoreo de comportamiento o reglas personalizadas de jugabilidad.

En el contexto de **SA-MP**, el **C-Bug** es una práctica que involucra acciones rápidas, como agacharse o cambiar de arma, después de disparar ciertas armas, reduciendo el tiempo de recarga y otorgando una ventaja injusta en combates. Este **include** detecta estas acciones de manera precisa, siendo útil en servidores **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, minijuegos o cualquier escenario que requiera un control riguroso de las mecánicas de juego. Su versatilidad permite usos más allá del anti-cheat, como análisis de jugabilidad o creación de mecánicas únicas.

> [!NOTE]
> **Cbug Detection** no se limita a bloquear **C-Bug**; puede integrarse en sistemas de monitoreo, eventos específicos o minijuegos con reglas personalizadas. **En resumen, son los desarrolladores quienes deciden qué acción debe ocurrir cuando un jugador es detectado usando C-Bug**, permitiendo una amplia flexibilidad en la aplicación de la detección según el contexto del servidor.

## Idiomas

- Português: [README](../../)
- Deutsch: [README](../Deutsch/README.md)
- English: [README](../English/README.md)
- Français: [README](../Francais/README.md)
- Italiano: [README](../Italiano/README.md)
- Polski: [README](../Polski/README.md)
- Русский: [README](../Русский/README.md)
- Svenska: [README](../Svenska/README.md)
- Türkçe: [README](../Turkce/README.md)

## Índice

- [Cbug Detection](#cbug-detection)
  - [Idiomas](#idiomas)
  - [Índice](#índice)
  - [Funcionalidades](#funcionalidades)
  - [Cómo Funciona](#cómo-funciona)
    - [Lógica de Detección](#lógica-de-detección)
    - [Eventos Monitoreados](#eventos-monitoreados)
    - [Configuraciones (Defines)](#configuraciones-defines)
  - [Estructura del Código](#estructura-del-código)
  - [Instalación y Configuración](#instalación-y-configuración)
    - [Instalación](#instalación)
    - [Configuración](#configuración)
  - [Cómo Utilizar](#cómo-utilizar)
    - [Activación por Jugador](#activación-por-jugador)
    - [Desactivación por Jugador](#desactivación-por-jugador)
    - [Verificación de Estado](#verificación-de-estado)
    - [Integración con Anti-Cheat](#integración-con-anti-cheat)
    - [Registro de Detecciones](#registro-de-detecciones)
    - [Ejemplo en Minijuego](#ejemplo-en-minijuego)
  - [Aplicaciones Posibles](#aplicaciones-posibles)
  - [Personalización](#personalización)
    - [Ajuste de Defines](#ajuste-de-defines)
    - [Personalización del Callback](#personalización-del-callback)
  - [Pruebas y Validación](#pruebas-y-validación)
  - [Limitaciones y Consideraciones](#limitaciones-y-consideraciones)
  - [Licencia](#licencia)
    - [Condiciones:](#condiciones)

## Funcionalidades

- **Detección Completa de C-Bug**: Identifica las siguientes variantes:
  - Clásico
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Disparos rápidos
- **Control por Jugador**: Activa o desactiva la detección individualmente para cada jugador.
- **Ajuste por Ping**: Adapta los tiempos de detección según el ping del jugador, minimizando falsos positivos.
- **Protección contra Falsos Positivos**: Valida el estado del jugador, arma, munición y animaciones para garantizar precisión.
- **Configuraciones Personalizables**: Define parámetros de detección a través de constantes (`defines`), aunque los valores predeterminados están optimizados.
- **Callback Extensible**: El callback `CbugDetection_OnDetected` permite integración con sistemas personalizados.
- **Ligero y Optimizado**: Usa solo `OnPlayerKeyStateChange` y `OnPlayerWeaponShot`, con un impacto mínimo en el rendimiento.

## Cómo Funciona

El include monitorea las acciones de los jugadores mediante los callbacks **OnPlayerKeyStateChange** y **OnPlayerWeaponShot**, utilizando un sistema de **puntuación de sospecha** (`cbug_d_suspicion_score`) para identificar patrones de C-Bug. Cuando la puntuación supera un límite configurable (`CBUG_D_SUSPICION_THRESHOLD`), se activa el callback `CbugDetection_OnDetected`.

> [!IMPORTANT]
> El sistema está diseñado para ser robusto, con validaciones rigurosas que minimizan **falsos positivos**, **aunque pueden ocurrir casos raros**, como se detalla en la sección de limitaciones.

### Lógica de Detección
1. **Puntuación de Sospecha**:
   - Las acciones sospechosas (e.g., agacharse, cambiar de arma, disparos rápidos) incrementan `cbug_d_suspicion_score` con pesos específicos.
   - Ejemplo: Agacharse después de disparar suma **4.0** puntos; disparos rápidos suman **3.0** puntos.
   - La puntuación disminuye con el tiempo (`CBUG_D_SUSPICION_DECAY = 0.5` por segundo) para evitar la acumulación de acciones no relacionadas.

2. **Ventanas de Tiempo**:
   - Las acciones se consideran sospechosas si ocurren justo después de un disparo (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Los disparos rápidos se detectan si ocurren dentro de `CBUG_D_SHOT_INTERVAL = 200ms`.

3. **Validaciones**:
   - Monitorea solo armas específicas (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - La detección se desactiva si:
     - El jugador no está a pie (`PLAYER_STATE_ONFOOT`).
     - El jugador está corriendo o saltando (verificado por animaciones).
     - No hay munición (`GetPlayerAmmo(playerid) <= 0`).
     - La detección está desactivada para el jugador (`cbug_d_player_status`).

4. **Ajuste por Ping**:
   - La función `CbugDetection_GetPingAdjtThresh` ajusta los tiempos según el ping:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Esto asegura justicia para jugadores con mayor latencia (e.g., **200**-**300ms**).

5. **Cooldown**:
   - Un intervalo (`CBUG_D_COOLDOWN_TIME = 1500ms`) evita detecciones repetidas para la misma secuencia.

### Eventos Monitoreados
- **OnPlayerKeyStateChange**:
  - Rastrea teclas presionadas (agacharse, correr, izquierda/derecha) y cambios de arma.
  - Detecta patrones como agacharse o rodar después de disparar.
  - Ejemplo:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Verificaciones adicionales para Rollbug, Quick C-switch, etc.
    }
    ```

- **OnPlayerWeaponShot**:
  - Monitorea disparos y secuencias rápidas.
  - Actualiza la puntuación para disparos consecutivos rápidos.
  - Ejemplo:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Inicializa el estado de detección del jugador como desactivado y reinicia variables:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Configuraciones (Defines)
Los defines controlan el comportamiento de la detección. Los valores predeterminados están optimizados y recomendados:

| Define                        | Valor Predeterminado | Descripción                                                                              |
|-------------------------------|----------------------|------------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Puntuación necesaria para la detección. Valores mayores reducen la sensibilidad.         |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Decaimiento de la puntuación por segundo.                                                |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Ventana (**ms**) para que las acciones después de un disparo se consideren sospechosas.  |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Ventana (**ms**) para detectar disparos rápidos consecutivos.                             |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Cooldown (**ms**) entre detecciones para evitar repeticiones.                             |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Multiplicador para el ajuste de ping en los tiempos.                                      |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Buffer (**ms**) para la detección de cambio de arma después de un disparo.                |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Tiempo (**ms**) después de la última acción para reiniciar la puntuación.                 |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | Armas monitoreadas (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).   |

> [!WARNING]
> No se recomienda modificar los valores de los defines, ya que los predeterminados fueron cuidadosamente calibrados para equilibrar precisión y rendimiento.

## Estructura del Código

El include está organizado en secciones distintas, cada una con una función específica:

1. **Encabezado**:
   - Define el include guard (`_cbug_detection_included`) y verifica la inclusión de `a_samp` o `open.mp`.

2. **Configuraciones (Defines)**:
   - Define constantes para parámetros de detección (e.g., `CBUG_D_SUSPICION_THRESHOLD`).
   - Usa `#if defined` para evitar conflictos de redefinición.

3. **Variables Globales**:
   - Almacena estados de los jugadores:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // Estado por jugador
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Puntuación de sospecha
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Último disparo
     // ... otras variables para rastreo
     ```

4. **Funciones Utilitarias (Static Stock)**:
   - Funciones internas para la lógica de detección:
     - `CbugDetection_GetPingAdjtThresh`: Ajusta tiempos por ping.
     - `CbugDetection_IsCbugWeapon`: Verifica armas monitoreadas.
     - `CbugDetection_IsJumping`: Detecta animaciones de salto.
     - `CbugDetection_IsRunning`: Detecta animaciones de carrera.
     - `CbugDetection_ResetVariables`: Reinicia variables del jugador.
   - Ejemplo:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... otras variables a reiniciar...

         return true;
     }
     ```

5. **Funciones Públicas (Stock)**:
   - Funciones para el control del sistema:
     - `CbugDetection_Player`: Activa o desactiva la detección para un jugador específico.
     - `CbugDetection_IsActive`: Verifica si la detección está activa para un jugador.
   - Ejemplo:
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
   - Implementa la lógica en `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect`, y `OnPlayerDisconnect`.
   - Proporciona `CbugDetection_OnDetected` para personalización.
   - Ejemplo:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **Hooks ALS**:
   - Integra callbacks estándar de SA-MP para compatibilidad:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... otros ALS a definir...
     ```

## Instalación y Configuración

### Instalación
1. **Descarga el Include**:
   - Coloca el archivo `cbug-detection.inc` en el directorio `pawno/include`.

2. **Inclúyelo en el Gamemode**:
   - Agrega al inicio del gamemode:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> Asegúrate de incluir `a_samp` o `open.mp` antes de `cbug-detection.inc`, o el compilador reportará errores.

3. **Compila el Gamemode**:
   - Usa el compilador **Pawn** para compilar el gamemode, verificando la ausencia de errores.

### Configuración
- **Define el Callback**:
  - Implementa `CbugDetection_OnDetected` en el gamemode para manejar las detecciones:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "¡Jugador %d detectado usando C-Bug!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Cómo Utilizar

El include ofrece control individual para la detección de C-Bug por jugador. A continuación, ejemplos prácticos de uso:

### Activación por Jugador
- Activa para un jugador específico, e.g., en un evento de combate:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Desactivación por Jugador
- Desactiva para un jugador específico, e.g., para administradores:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Verificación de Estado
- Verifica si la detección está activa para un jugador:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "La detección está activa para ti.");
  ```

### Integración con Anti-Cheat
- Aplica sanciones en `CbugDetection_OnDetected`:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "¡Jugador %s (ID: %d) detectado usando C-Bug!", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Registro de Detecciones
- Registra las detecciones en un archivo:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Jugador %s (ID: %d) detectado usando C-Bug\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Ejemplo en Minijuego
- Activa la detección solo para jugadores específicos en un minijuego:
  ```pawn
  CMD:iniciarminijuego(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "¡Detección de C-Bug activada para ti en el minijuego!");

      return 1;
  }

  CMD:finalizarminijuego(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "¡Detección de C-Bug desactivada para ti!");

      return 1;
  }
  ```

> [!TIP]
> Usa `CbugDetection_IsActive` para verificar el estado de la detección antes de ejecutar acciones condicionales, como alertas o sanciones.

## Aplicaciones Posibles

**Cbug Detection** es altamente versátil y puede usarse en diversos contextos:

- **Servidores Roleplay (RP)**: Refuerza las reglas contra **C-Bug** para combates realistas.
- **Servidores TDM/DM**: Garantiza equilibrio en combates detectando **C-Bug**.
- **Monitoreo**: Analiza el comportamiento de los jugadores o identifica infractores frecuentes.
- **Minijuegos y Eventos**: Controla **C-Bug** solo en eventos específicos.
- **Mecánicas Personalizadas**: Integra con sistemas de puntuación o penalidades.

> [!NOTE]
> La capacidad de preservar configuraciones individuales al activar o desactivar globalmente hace que el include sea ideal para servidores con reglas dinámicas.

## Personalización

Aunque el include está optimizado con configuraciones predeterminadas, es posible ajustar los defines o el callback `CbugDetection_OnDetected`.

> [!WARNING]
> Modificar los defines u otras configuraciones **no es recomendable**, ya que los valores predeterminados fueron probados y equilibrados para máxima eficacia. Las modificaciones pueden causar falsos positivos, detecciones imprecisas o incluso impedir el funcionamiento adecuado.

### Ajuste de Defines
- **Servidores con Ping Alto**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Detección Más Estricta**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Armas Personalizadas**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Personalización del Callback
- Agrega lógica avanzada:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Advertencia %d: ¡Jugador %s (ID: %d) detectado usando C-Bug!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Uso repetido de C-Bug");
      
      return 1;
  }
  ```

## Pruebas y Validación

Para garantizar el funcionamiento correcto, realiza las siguientes pruebas:

- **Detección de C-Bug**:
  - Prueba todas las variantes listadas con armas monitoreadas.
  - Confirma que `CbugDetection_OnDetected` se activa después de **2**-**3** secuencias.

- **Falsos Positivos**:
  - Realiza acciones como correr, saltar o disparar sin agacharse.
  - Verifica que no haya detecciones para acciones no relacionadas con **C-Bug**.

- **Control por Jugador**:
  - Prueba `CbugDetection_Player` para activación/desactivación por jugador.
  - Confirma que la detección solo ocurre para jugadores con `cbug_d_player_status` activo.

- **Verificación de Estado**:
  - Usa `CbugDetection_IsActive` para confirmar el estado por jugador.

- **OnPlayerConnect**:
  - Verifica que los nuevos jugadores inicien con la detección desactivada (`cbug_d_player_status = false`).

- **Ping Alto**:
  - Simula un ping de **200**-**300ms** y verifica la precisión de la detección.

- **Rendimiento**:
  - Prueba con **50**+ jugadores para confirmar la eficiencia.

> [!TIP]
> Realiza pruebas en un entorno controlado con pocos jugadores antes de implementar en servidores con muchos usuarios.

## Limitaciones y Consideraciones

- **Dependencia de Callbacks**:
  - Depende de `OnPlayerWeaponShot` y `OnPlayerKeyStateChange`, que pueden verse afectados por lag u otros includes.
- **Ping Extremo**:
  - Latencias muy altas (>**500ms**) pueden requerir ajustes en `CBUG_D_PING_MULTIPLIER`.
- **Animaciones**:
  - La detección de carrera/salto usa índices de animación, que pueden variar en clientes modificados.
- **Falsos Positivos**:
  - A pesar de ser robusto, **los falsos positivos pueden ocurrir en casos raros**, como acciones legítimas en secuencia rápida, lo cual es considerado normal.

> [!IMPORTANT]
> **Los falsos positivos** se minimizan mediante validaciones, pero pueden ocurrir en escenarios atípicos. Monitorea las detecciones para ajustar el uso, si es necesario.

## Licencia

Este include está protegido bajo la Licencia Apache 2.0, que permite:

- ✔️ Uso comercial y privado
- ✔️ Modificación del código fuente
- ✔️ Distribución del código
- ✔️ Concesión de patentes

### Condiciones:

- Mantener el aviso de derechos de autor
- Documentar cambios significativos
- Incluir copia de la licencia Apache 2.0

Para más detalles sobre la licencia: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - Todos los derechos reservados**