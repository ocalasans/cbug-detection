# Cbug Detection

O **Cbug Detection** é um include flexível para **San Andreas Multiplayer (SA-MP)** projetado para detectar o **exploit C-Bug**, uma técnica que permite aos jogadores atirar mais rápido do que o previsto ao explorar mecânicas do jogo. Este **include** oferece aos **desenvolvedores de servidores** uma ferramenta confiável para identificar o uso de **C-Bug**, possibilitando integração com sistemas como anti-cheat, monitoramento de comportamento ou regras personalizadas de jogabilidade.

No contexto do **SA-MP**, o **C-Bug** é uma prática que envolve ações rápidas, como agachar ou trocar de arma, após disparar certas armas, reduzindo o tempo de recarga e conferindo vantagem injusta em combates. Este **include** detecta essas ações de forma precisa, sendo útil em servidores **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, minigames ou qualquer cenário que exija controle rigoroso das mecânicas de jogo. Sua versatilidade permite usos além do anti-cheat, como análise de jogabilidade ou criação de mecânicas únicas.

> [!NOTE]
> O **Cbug Detection** não se limita a bloquear **C-Bug**; ele pode ser integrado em sistemas de monitoramento, eventos específicos ou minigames com regras customizadas. **Em resumo, são os desenvolvedores que decidem qual ação deve ocorrer quando um player for detectado usando C-Bug**, permitindo uma ampla flexibilidade na aplicação da detecção conforme o contexto do servidor.

## Idiomas

- Deutsch: [README](translations/Deutsch/README.md)
- English: [README](translations/English/README.md)
- Español: [README](translations/Espanol/README.md)
- Français: [README](translations/Francais/README.md)
- Italiano: [README](translations/Italiano/README.md)
- Polski: [README](translations/Polski/README.md)
- Русский: [README](translations/Русский/README.md)
- Svenska: [README](translations/Svenska/README.md)
- Türkçe: [README](translations/Turkce/README.md)

## Índice

- [Cbug Detection](#cbug-detection)
  - [Idiomas](#idiomas)
  - [Índice](#índice)
  - [Funcionalidades](#funcionalidades)
  - [Como Funciona](#como-funciona)
    - [Lógica de Detecção](#lógica-de-detecção)
    - [Eventos Monitorados](#eventos-monitorados)
    - [Configurações (Defines)](#configurações-defines)
  - [Estrutura do Código](#estrutura-do-código)
  - [Instalação e Configuração](#instalação-e-configuração)
    - [Instalação](#instalação)
    - [Configuração](#configuração)
  - [Como Utilizar](#como-utilizar)
    - [Ativação por Jogador](#ativação-por-jogador)
    - [Desativação por Jogador](#desativação-por-jogador)
    - [Verificação de Estado](#verificação-de-estado)
    - [Integração com Anti-Cheat](#integração-com-anti-cheat)
    - [Registro de Detecções](#registro-de-detecções)
    - [Exemplo em Minigame](#exemplo-em-minigame)
  - [Aplicações Possíveis](#aplicações-possíveis)
  - [Personalização](#personalização)
    - [Ajuste de Defines](#ajuste-de-defines)
    - [Personalização do Callback](#personalização-do-callback)
  - [Testes e Validação](#testes-e-validação)
  - [Limitações e Considerações](#limitações-e-considerações)
  - [Licença](#licença)
    - [Condições:](#condições)

## Funcionalidades

- **Detecção Abrangente de C-Bug**: Identifica as seguintes variantes:
  - Clássico
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Disparos rápidos
- **Controle por Jogador**: Ativa ou desativa a detecção individualmente para cada jogador.
- **Ajuste por Ping**: Adapta os tempos de detecção com base no ping do jogador, minimizando falsos positivos.
- **Proteção contra Falsos Positivos**: Valida estado do jogador, arma, munição e animações para garantir precisão.
- **Configurações Personalizáveis**: Define parâmetros de detecção via constantes (`defines`), embora os valores padrão sejam otimizados.
- **Callback Extensível**: O callback `CbugDetection_OnDetected` permite integração com sistemas personalizados.
- **Leve e Otimizado**: Usa apenas `OnPlayerKeyStateChange` e `OnPlayerWeaponShot`, com impacto mínimo no desempenho.

## Como Funciona

O include monitora ações dos jogadores por meio dos callbacks **OnPlayerKeyStateChange** e **OnPlayerWeaponShot**, utilizando um sistema de **pontuação de suspeita** (`cbug_d_suspicion_score`) para identificar padrões de C-Bug. Quando a pontuação ultrapassa um limite configurável (`CBUG_D_SUSPICION_THRESHOLD`), o callback `CbugDetection_OnDetected` é acionado.

> [!IMPORTANT]
> O sistema é projetado para ser robusto, com validações rigorosas que minimizam **falsos positivos**, **embora casos raros possam ocorrer**, conforme detalhado na seção de limitações.

### Lógica de Detecção
1. **Pontuação de Suspeita**:
   - Ações suspeitas (e.g., agachar, trocar arma, disparos rápidos) incrementam `cbug_d_suspicion_score` com pesos específicos.
   - Exemplo: Agachar após disparar adiciona **4.0** pontos; disparos rápidos adicionam **3.0** pontos.
   - A pontuação decai com o tempo (`CBUG_D_SUSPICION_DECAY = 0.5` por segundo) para evitar acumulação de ações não relacionadas.

2. **Janelas de Tempo**:
   - Ações são consideradas suspeitas se ocorrerem logo após um disparo (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Disparos rápidos são detectados se ocorrerem dentro de `CBUG_D_SHOT_INTERVAL = 200ms`.

3. **Validações**:
   - Monitora apenas armas específicas (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - A detecção é desativada se:
     - O jogador não está a pé (`PLAYER_STATE_ONFOOT`).
     - O jogador está correndo ou pulando (verificado por animações).
     - Não há munição (`GetPlayerAmmo(playerid) <= 0`).
     - A detecção está desativada para o jogador (`cbug_d_player_status`).

4. **Ajuste por Ping**:
   - A função `CbugDetection_GetPingAdjtThresh` ajusta os tempos com base no ping:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Isso garante justiça para jogadores com maior latência (e.g., **200**-**300ms**).

5. **Cooldown**:
   - Um intervalo (`CBUG_D_COOLDOWN_TIME = 1500ms`) evita detecções repetidas para a mesma sequência.

### Eventos Monitorados
- **OnPlayerKeyStateChange**:
  - Rastreia teclas pressionadas (agachar, correr, esquerda/direita) e trocas de arma.
  - Detecta padrões como agachar ou rolar após disparar.
  - Exemplo:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Verificações adicionais para Rollbug, Quick C-switch, etc.
    }
    ```

- **OnPlayerWeaponShot**:
  - Monitora disparos e sequências rápidas.
  - Atualiza a pontuação para disparos consecutivos rápidos.
  - Exemplo:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Inicializa o estado de detecção do jogador como desativado e reseta variáveis:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Configurações (Defines)
Os defines controlam o comportamento da detecção. Os valores padrão são otimizados e recomendados:

| Define                        | Valor Padrão         | Descrição                                                                              |
|-------------------------------|----------------------|----------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Pontuação necessária para detecção. Valores maiores reduzem sensibilidade.             |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Decaimento da pontuação por segundo.                                                   |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Janela (**ms**) para ações após disparo serem consideradas suspeitas.                  |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Janela (**ms**) para detectar disparos rápidos consecutivos.                           |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Cooldown (**ms**) entre detecções para evitar repetições.                              |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Multiplicador para ajuste de ping nos tempos.                                          |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Buffer (**ms**) para detecção de troca de arma após disparo.                           |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Tempo (**ms**) após última ação para resetar pontuação.                                |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | Armas monitoradas (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**). |

> [!WARNING]
> Não é recomendado alterar os valores dos defines, pois os padrões foram cuidadosamente calibrados para balancear precisão e desempenho.

## Estrutura do Código

O include é organizado em seções distintas, cada uma com uma função específica:

1. **Cabeçalho**:
   - Define o include guard (`_cbug_detection_included`) e verifica a inclusão de `a_samp` ou `open.mp`.

2. **Configurações (Defines)**:
   - Define constantes para parâmetros de detecção (e.g., `CBUG_D_SUSPICION_THRESHOLD`).
   - Usa `#if defined` para evitar conflitos de redefinição.

3. **Variáveis Globais**:
   - Armazena estados dos jogadores:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // Estado por jogador
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Pontuação de suspeita
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Último disparo
     // ... outras variáveis para rastreamento
     ```

4. **Funções Utilitárias (Static Stock)**:
   - Funções internas para lógica de detecção:
     - `CbugDetection_GetPingAdjtThresh`: Ajusta tempos por ping.
     - `CbugDetection_IsCbugWeapon`: Verifica armas monitoradas.
     - `CbugDetection_IsJumping`: Detecta animações de pulo.
     - `CbugDetection_IsRunning`: Detecta animações de corrida.
     - `CbugDetection_ResetVariables`: Reseta variáveis do jogador.
   - Exemplo:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... outras variáveis a ser resetadas...

         return true;
     }
     ```

5. **Funções Públicas (Stock)**:
   - Funções para controle do sistema:
     - `CbugDetection_Player`: Ativa ou desativa a detecção para um jogador específico.
     - `CbugDetection_IsActive`: Verifica se a detecção está ativa para um jogador.
   - Exemplo:
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
   - Implementa a lógica em `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect`, e `OnPlayerDisconnect`.
   - Fornece `CbugDetection_OnDetected` para personalização.
   - Exemplo:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **Hooks ALS**:
   - Integra callbacks padrão do SA-MP para compatibilidade:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... outros ALS a serem definidos...
     ```

## Instalação e Configuração

### Instalação
1. **Baixe o Include**:
   - Coloque o arquivo `cbug-detection.inc` no diretório `pawno/include`.

2. **Inclua no Gamemode**:
   - Adicione ao início do gamemode:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> Certifique-se de incluir `a_samp` ou `open.mp` antes do `cbug-detection.inc`, ou o compilador reportará erros.

3. **Compile o Gamemode**:
   - Use o compilador **Pawn** para compilar o gamemode, verificando a ausência de erros.

### Configuração
- **Defina o Callback**:
  - Implemente `CbugDetection_OnDetected` no gamemode para lidar com detecções:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];

        format(string, sizeof(string), "Jogador %d detectado usando C-Bug!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Como Utilizar

O include oferece controle individual para detecção de C-Bug por jogador. Abaixo, exemplos práticos de uso:

### Ativação por Jogador
- Ative para um jogador específico, e.g., em um evento de combate:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Desativação por Jogador
- Desative para um jogador específico, e.g., para administradores:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Verificação de Estado
- Verifique se a detecção está ativa para um jogador:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "Detecção está ativa para você.");
  ```

### Integração com Anti-Cheat
- Aplique punições em `CbugDetection_OnDetected`:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Jogador %s (ID: %d) detectado usando C-Bug!", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Registro de Detecções
- Registre detecções em um arquivo:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Jogador %s (ID: %d) detectado usando C-Bug\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Exemplo em Minigame
- Ative a detecção apenas para jogadores específicos em um minigame:
  ```pawn
  CMD:iniciarminigame(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "Detecção de C-Bug ativada para você no minigame!");

      return 1;
  }

  CMD:finalizarminigame(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "Detecção de C-Bug desativada para você!");

      return 1;
  }
  ```

> [!TIP]
> Use `CbugDetection_IsActive` para verificar o estado da detecção antes de executar ações condicionais, como alertas ou punições.

## Aplicações Possíveis

O **Cbug Detection** é altamente versátil e pode ser usado em diversos contextos:

- **Servidores Roleplay (RP)**: Reforce regras contra **C-Bug** para combates realistas.
- **Servidores TDM/DM**: Garanta equilíbrio em combates detectando **C-Bug**.
- **Monitoramento**: Analise comportamento dos jogadores ou identifique infratores frequentes.
- **Minigames e Eventos**: Controle **C-Bug** apenas em eventos específicos.
- **Mecânicas Personalizadas**: Integre com sistemas de pontuação ou penalidades.

> [!NOTE]
> A capacidade de preservar configurações individuais ao ativar ou desativar globalmente torna o include ideal para servidores com regras dinâmicas.

## Personalização

Embora o include seja otimizado com configurações padrão, é possível ajustar os defines ou o callback `CbugDetection_OnDetected`.

> [!WARNING]
> Alterar os defines ou outras configurações **não é recomendado**, pois os valores padrão foram testados e balanceados para máxima eficácia. Modificações podem causar falsos positivos, detecções imprecisas ou até mesmo impedir o funcionamento adequado.

### Ajuste de Defines
- **Servidores com Ping Alto**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Detecção Mais Rigorosa**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Armas Personalizadas**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Personalização do Callback
- Adicione lógica avançada:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Aviso %d: Jogador %s (ID: %d) detectado usando C-Bug!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Uso repetido de C-Bug");
      
      return 1;
  }
  ```

## Testes e Validação

Para garantir o funcionamento correto, execute os seguintes testes:

- **Detecção de C-Bug**:
  - Teste todas as variantes listadas com armas monitoradas.
  - Confirme que `CbugDetection_OnDetected` é acionado após **2**-**3** sequências.

- **Falsos Positivos**:
  - Execute ações como correr, pular ou atirar sem agachar.
  - Verifique que não há detecções para ações não relacionadas ao **C-Bug**.

- **Controle por Jogador**:
  - Teste `CbugDetection_Player` para ativação/desativação por jogador.
  - Confirme que a detecção só ocorre para jogadores com `cbug_d_player_status` ativo.

- **Verificação de Estado**:
  - Use `CbugDetection_IsActive` para confirmar o estado por jogador.

- **OnPlayerConnect**:
  - Verifique se novos jogadores iniciam com detecção desativada (`cbug_d_player_status = false`).

- **Ping Alto**:
  - Simule ping de **200**-**300ms** e verifique a precisão da detecção.

- **Desempenho**:
  - Teste com **50**+ jogadores para confirmar eficiência.

> [!TIP]
> Teste em um ambiente controlado com poucos jogadores antes de implantar em servidores lotados.

## Limitações e Considerações

- **Dependência de Callbacks**:
  - Depende de `OnPlayerWeaponShot` e `OnPlayerKeyStateChange`, que podem ser afetados por lag ou outros includes.
- **Ping Extremo**:
  - Latências muito altas (>**500ms**) podem exigir ajustes em `CBUG_D_PING_MULTIPLIER`.
- **Animações**:
  - A detecção de corrida/pulo usa índices de animação, que podem variar em clientes modificados.
- **Falsos Positivos**:
  - Apesar de robusto, **falsos positivos podem ocorrer em casos raros**, como ações legítimas em sequência rápida, o que é considerado normal.

> [!IMPORTANT]
> **Falsos positivos** são minimizados pelas validações, mas podem ocorrer em cenários atípicos. Monitore detecções para ajustar o uso, se necessário.

## Licença

Este include está protegido sob a Licença Apache 2.0, que permite:

- ✔️ Uso comercial e privado
- ✔️ Modificação do código fonte
- ✔️ Distribuição do código
- ✔️ Concessão de patentes

### Condições:

- Manter o aviso de direitos autorais
- Documentar alterações significativas
- Incluir cópia da licença Apache 2.0

Para mais detalhes sobre a licença: http://www.apache.org/licenses/LICENSE-2.0

**Copyright (c) Calasans - Todos os direitos reservados**