# Cbug Detection

**Cbug Detection**, **San Andreas Multiplayer (SA-MP)** için tasarlanmış esnek bir include dosyasıdır ve oyuncuların oyun mekaniklerini istismar ederek normalden daha hızlı ateş etmelerini sağlayan **C-Bug** exploitini tespit etmek için kullanılır. Bu **include**, **sunucu geliştiricilerine** **C-Bug** kullanımını güvenilir bir şekilde tespit etme imkanı sunar ve anti-hile sistemleri, davranış izleme veya özel oyun kurallarıyla entegrasyon sağlar.

**SA-MP** bağlamında, **C-Bug**, oyuncuların belirli silahlarla ateş ettikten sonra hızlıca çömelme veya silah değiştirme gibi eylemler yaparak yeniden yükleme süresini azaltıp savaşlarda haksız avantaj elde ettiği bir tekniktir. Bu **include**, bu eylemleri hassas bir şekilde tespit eder ve **Roleplay (RP)**, **Team Deathmatch (TDM)**, **Deathmatch (DM)**, mini oyunlar veya oyun mekaniklerinin sıkı kontrol gerektirdiği herhangi bir senaryoda kullanışlıdır. Esnekliği, anti-hile sistemlerinin ötesinde kullanım imkanı sunar; örneğin, oyun analizi veya benzersiz mekanikler oluşturma gibi.

> [!NOTE]
> **Cbug Detection**, yalnızca **C-Bug**'u engellemekle sınırlı değildir; izleme sistemleri, özel etkinlikler veya özel kurallı mini oyunlarla entegre edilebilir. **Özetle, bir oyuncunun C-Bug kullandığı tespit edildiğinde hangi eylemin gerçekleşeceği geliştiricilere bağlıdır**, bu da sunucu bağlamına göre geniş bir esneklik sağlar.

## Diller

- Português: [README](../../)
- Deutsch: [README](../Deutsch/README.md)
- English: [README](../English/README.md)
- Español: [README](../Espanol/README.md)
- Français: [README](../Francais/README.md)
- Italiano: [README](../Italiano/README.md)
- Polski: [README](../Polski/README.md)
- Русский: [README](../Русский/README.md)
- Svenska: [README](../Svenska/README.md)

## İçindekiler

- [Cbug Detection](#cbug-detection)
  - [Diller](#diller)
  - [İçindekiler](#i̇çindekiler)
  - [Özellikler](#özellikler)
  - [Nasıl Çalışır](#nasıl-çalışır)
    - [Tespit Mantığı](#tespit-mantığı)
    - [İzlenen Olaylar](#i̇zlenen-olaylar)
    - [Ayarlar (Defines)](#ayarlar-defines)
  - [Kod Yapısı](#kod-yapısı)
  - [Kurulum ve Yapılandırma](#kurulum-ve-yapılandırma)
    - [Kurulum](#kurulum)
    - [Yapılandırma](#yapılandırma)
  - [Nasıl Kullanılır](#nasıl-kullanılır)
    - [Oyuncu Bazında Aktivasyon](#oyuncu-bazında-aktivasyon)
    - [Oyuncu Bazında Devre Dışı Bırakma](#oyuncu-bazında-devre-dışı-bırakma)
    - [Durum Kontrolü](#durum-kontrolü)
    - [Anti-Hile ile Entegrasyon](#anti-hile-ile-entegrasyon)
    - [Tespit Kayıtları](#tespit-kayıtları)
    - [Mini Oyun Örneği](#mini-oyun-örneği)
  - [Olası Kullanım Alanları](#olası-kullanım-alanları)
  - [Özelleştirme](#özelleştirme)
    - [Defines Ayarları](#defines-ayarları)
    - [Callback Özelleştirme](#callback-özelleştirme)
  - [Test ve Doğrulama](#test-ve-doğrulama)
  - [Sınırlamalar ve Dikkat Edilmesi Gerekenler](#sınırlamalar-ve-dikkat-edilmesi-gerekenler)
  - [Lisans](#lisans)
    - [Koşullar:](#koşullar)

## Özellikler

- **Kapsamlı C-Bug Tespiti**: Şu varyantları tespit eder:
  - Klasik
  - Rollbug
  - Quick C-switch
  - Jumpbug
  - Run Bug
  - Slide C-Bug
  - Hızlı ateş
- **Oyuncu Bazında Kontrol**: Her oyuncu için tespiti bireysel olarak etkinleştirir veya devre dışı bırakır.
- **Ping'e Göre Ayar**: Oyuncunun ping'ine bağlı olarak tespit sürelerini ayarlar, yanlış pozitifleri en aza indirir.
- **Yanlış Pozitiflere Karşı Koruma**: Oyuncunun durumu, silahı, cephanesi ve animasyonları doğrulayarak hassasiyet sağlar.
- **Özelleştirilebilir Ayarlar**: Tespit parametrelerini sabitler (`defines`) aracılığıyla tanımlar, ancak varsayılan değerler optimize edilmiştir.
- **Genişletilebilir Callback**: `CbugDetection_OnDetected` callback'i, özel sistemlerle entegrasyon sağlar.
- **Hafif ve Optimize**: Yalnızca `OnPlayerKeyStateChange` ve `OnPlayerWeaponShot` kullanır, performansa minimum etki eder.

## Nasıl Çalışır

Include, oyuncuların eylemlerini **OnPlayerKeyStateChange** ve **OnPlayerWeaponShot** callback'leri aracılığıyla izler ve **şüphe puanı** (`cbug_d_suspicion_score`) sistemi kullanarak C-Bug kalıplarını tespit eder. Şüphe puanı yapılandırılabilir bir eşiği (`CBUG_D_SUSPICION_THRESHOLD`) aştığında, `CbugDetection_OnDetected` callback'i tetiklenir.

> [!IMPORTANT]
> Sistem, **yanlış pozitifleri** en aza indirecek şekilde sağlam doğrulamalarla tasarlanmıştır, **ancak nadir durumlarda yanlış pozitifler olabilir**, sınırlamalar bölümünde detaylı olarak açıklanmıştır.

### Tespit Mantığı
1. **Şüphe Puanı**:
   - Şüpheli eylemler (örn. çömelme, silah değiştirme, hızlı ateş) `cbug_d_suspicion_score`'u belirli ağırlıklarla artırır.
   - Örnek: Ateş ettikten sonra çömelme **4.0** puan ekler; hızlı ateş **3.0** puan ekler.
   - Şüphe puanı zamanla azalır (`CBUG_D_SUSPICION_DECAY = 0.5` saniyede) ve ilgisiz eylemlerin birikmesini önler.

2. **Zaman Pencereleri**:
   - Eylemler, ateşten sonra belirli bir süre içinde gerçekleşirse şüpheli kabul edilir (`CBUG_D_SEQUENCE_INTERVAL = 1500ms`).
   - Hızlı ateş, `CBUG_D_SHOT_INTERVAL = 200ms` içinde gerçekleşirse tespit edilir.

3. **Doğrulamalar**:
   - Yalnızca belirli silahlar izlenir (`CBUG_D_VALID_WEAPONS`: **Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**).
   - Tespit, şu durumlarda devre dışı bırakılır:
     - Oyuncu yaya değilse (`PLAYER_STATE_ONFOOT`).
     - Oyuncu koşuyor veya zıplıyorsa (animasyonlarla kontrol edilir).
     - Cephane yoksa (`GetPlayerAmmo(playerid) <= 0`).
     - Oyuncu için tespit devre dışıysa (`cbug_d_player_status`).

4. **Ping Ayarı**:
   - `CbugDetection_GetPingAdjtThresh` fonksiyonu, ping'e göre süreleri ayarlar:
     ```pawn
     static stock Float:CbugDetection_GetPingAdjtThresh(playerid, cbug_d_base_threshold)
         return float(cbug_d_base_threshold) + (float(GetPlayerPing(playerid)) * CBUG_D_PING_MULTIPLIER);
     ```
   - Bu, yüksek gecikmeli oyuncular (örn. **200**-**300ms**) için adalet sağlar.

5. **Soğuma Süresi**:
   - Aynı diziyi tekrar tespit etmeyi önlemek için bir aralık (`CBUG_D_COOLDOWN_TIME = 1500ms`) kullanılır.

### İzlenen Olaylar
- **OnPlayerKeyStateChange**:
  - Basılan tuşları (çömelme, koşma, sola/sağa hareket) ve silah değişimlerini izler.
  - Ateşten sonra çömelme veya yuvarlanma gibi kalıpları tespit eder.
  - Örnek:
    ```pawn
    if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) {
        cbug_d_score_increment = 4.0;
        // Rollbug, Quick C-switch vb. için ek kontroller
    }
    ```

- **OnPlayerWeaponShot**:
  - Ateşleri ve hızlı dizileri izler.
  - Hızlı ardışık ateşler için şüphe puanını günceller.
  - Örnek:
    ```pawn
    if (cbug_d_time_since_last_shot <= cbug_d_adjusted_shot_threshold) {
        cbug_d_suspicion_score[playerid] += 3.0;
        cbug_d_shot_count[playerid]++;
        cbug_d_last_action_time[playerid] = cbug_d_current_time;
    }
    ```

- **OnPlayerConnect**:
  - Oyuncunun tespit durumunu devre dışı olarak başlatır ve değişkenleri sıfırlar:
    ```pawn
    cbug_d_player_status[playerid] = false;
    CbugDetection_ResetVariables(playerid);
    ```

### Ayarlar (Defines)
Defines, tespit davranışını kontrol eder. Varsayılan değerler optimize edilmiştir ve önerilir:

| Define                        | Varsayılan Değer     | Açıklama                                                                              |
|-------------------------------|----------------------|---------------------------------------------------------------------------------------|
| `CBUG_D_SUSPICION_THRESHOLD`  | 10.0                 | Tespit için gerekli şüphe puanı. Daha yüksek değerler hassasiyeti azaltır.            |
| `CBUG_D_SUSPICION_DECAY`      | 0.5                  | Saniyede şüphe puanının azalması.                                                    |
| `CBUG_D_SEQUENCE_INTERVAL`    | 1500                 | Ateş sonrası eylemlerin şüpheli sayıldığı zaman penceresi (**ms**).                   |
| `CBUG_D_SHOT_INTERVAL`        | 200                  | Hızlı ardışık ateşleri tespit için zaman penceresi (**ms**).                          |
| `CBUG_D_COOLDOWN_TIME`        | 1500                 | Tekrarlanan tespitleri önlemek için soğuma süresi (**ms**).                           |
| `CBUG_D_PING_MULTIPLIER`      | 0.01                 | Ping ayarları için çarpan.                                                            |
| `CBUG_D_WEAPON_BUFFER_TIME`   | 500                  | Ateş sonrası silah değişimi için tampon süresi (**ms**).                              |
| `CBUG_D_SCORE_RESET_TIME`     | 2000                 | Son eylemden sonra puanın sıfırlanması için süre (**ms**).                            |
| `CBUG_D_VALID_WEAPONS`        | {24, 25, 27, 33, 34} | İzlenen silahlar (**Desert Eagle**, **Shotgun**, **SPAS-12**, **Rifle**, **Sniper**). |

> [!WARNING]
> Defines değerlerini değiştirmek önerilmez, çünkü varsayılanlar hassasiyet ve performans için dikkatle dengelenmiştir.

## Kod Yapısı

Include, her biri belirli bir işlevi olan farklı bölümlerle düzenlenmiştir:

1. **Başlık**:
   - Include guard (`_cbug_detection_included`) tanımlar ve `a_samp` veya `open.mp` dahil edilmesini kontrol eder.

2. **Ayarlar (Defines)**:
   - Tespit parametreleri için sabitler tanımlar (örn. `CBUG_D_SUSPICION_THRESHOLD`).
   - `#if defined` ile yeniden tanımlama çatışmalarını önler.

3. **Genel Değişkenler**:
   - Oyuncuların durumlarını depolar:
     ```pawn
     static bool:cbug_d_player_status[MAX_PLAYERS]; // Oyuncu başına durum
     static Float:cbug_d_suspicion_score[MAX_PLAYERS]; // Şüphe puanı
     static cbug_d_last_fire_time[MAX_PLAYERS]; // Son ateş
     // ... diğer izleme değişkenleri
     ```

4. **Yardımcı Fonksiyonlar (Static Stock)**:
   - Tespit mantığı için dahili fonksiyonlar:
     - `CbugDetection_GetPingAdjtThresh`: Ping'e göre süreleri ayarlar.
     - `CbugDetection_IsCbugWeapon`: İzlenen silahları kontrol eder.
     - `CbugDetection_IsJumping`: Zıplama animasyonlarını tespit eder.
     - `CbugDetection_IsRunning`: Koşma animasyonlarını tespit eder.
     - `CbugDetection_ResetVariables`: Oyuncu değişkenlerini sıfırlar.
   - Örnek:
     ```pawn
     static stock bool:CbugDetection_ResetVariables(playerid)
     {
         if (cbug_d_suspicion_score[playerid] != 0.0)
             cbug_d_suspicion_score[playerid] = 0.0;

         if (cbug_d_last_fire_time[playerid] != 0)
             cbug_d_last_fire_time[playerid] = 0;

         if (cbug_d_last_roll_time[playerid] != 0)
             cbug_d_last_roll_time[playerid] = 0;

         // ... diğer sıfırlanacak değişkenler...

         return true;
     }
     ```

5. **Genel Fonksiyonlar (Stock)**:
   - Sistemi kontrol eden fonksiyonlar:
     - `CbugDetection_Player`: Belirli bir oyuncu için tespiti etkinleştirir/devre dışı bırakır.
     - `CbugDetection_IsActive`: Bir oyuncu için tespit durumunu kontrol eder.
   - Örnek:
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

6. **Callback'ler**:
   - `OnPlayerKeyStateChange`, `OnPlayerWeaponShot`, `OnPlayerConnect` ve `OnPlayerDisconnect` içinde mantığı uygular.
   - Özelleştirme için `CbugDetection_OnDetected` sağlar.
   - Örnek:
     ```pawn
     forward CbugDetection_OnDetected(playerid);
     ```

7. **ALS Hooks**:
   - SA-MP standart callback'lerini entegre eder:
     ```pawn
     #if defined _ALS_OnPlayerConnect
         #undef OnPlayerConnect
     #else
         #define _ALS_OnPlayerConnect
     #endif
     #define OnPlayerConnect CBUG_D_OnPlayerConnect

     // ... diğer ALS tanımları...
     ```

## Kurulum ve Yapılandırma

### Kurulum
1. **Include'u İndirin**:
   - `cbug-detection.inc` dosyasını `pawno/include` dizinine yerleştirin.

2. **Gamemode'a Ekleyin**:
   - Gamemode'un başına ekleyin:
     ```pawn
     #include <a_samp>
     #include <cbug-detection>
     ```

> [!WARNING]
> `a_samp` veya `open.mp`'yi `cbug-detection.inc`'den önce eklediğinizden emin olun, aksi takdirde derleyici hata bildirir.

3. **Gamemode'u Derleyin**:
   - **Pawn** derleyicisini kullanarak gamemode'u derleyin ve hata olmadığından emin olun.

### Yapılandırma
- **Callback'i Tanımlayın**:
  - Tespitleri işlemek için gamemode'da `CbugDetection_OnDetected` uygulayın:
    ```pawn
    public CbugDetection_OnDetected(playerid) {
        new string[100];
        
        format(string, sizeof(string), "Oyuncu %d C-Bug kullanırken tespit edildi!", playerid);
        SendClientMessageToAll(-1, string);

        return 1;
    }
    ```

## Nasıl Kullanılır

Include, C-Bug tespiti için oyuncu bazında kontrol sunar. Aşağıda pratik kullanım örnekleri verilmiştir:

### Oyuncu Bazında Aktivasyon
- Belirli bir oyuncu için etkinleştirin, örneğin bir savaş etkinliğinde:
  ```pawn
  CbugDetection_Player(playerid, true);
  ```

### Oyuncu Bazında Devre Dışı Bırakma
- Belirli bir oyuncu için devre dışı bırakın, örneğin yöneticiler için:
  ```pawn
  CbugDetection_Player(playerid, false);
  ```

### Durum Kontrolü
- Bir oyuncu için tespit durumunu kontrol edin:
  ```pawn
  if (CbugDetection_IsActive(playerid))
      SendClientMessage(playerid, -1, "Tespit sizin için aktif.");
  ```

### Anti-Hile ile Entegrasyon
- `CbugDetection_OnDetected`'da cezalar uygulayın:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[110], name[MAX_PLAYER_NAME];
      GetPlayerName(playerid, name, sizeof(name));

      format(string, sizeof(string), "Oyuncu %s (ID: %d) C-Bug kullanırken tespit edildi!", name, playerid);
      SendClientMessageToAll(-1, string);

      Kick(playerid);

      return 1;
  }
  ```

### Tespit Kayıtları
- Tespitleri bir dosyaya kaydedin:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME], 
          File:log = fopen("cbug_detections.log", io_append);

      format(string, sizeof(string), "[%s] Oyuncu %s (ID: %d) C-Bug kullanırken tespit edildi\n", GetCurrentDateTime(), name, playerid);

      fwrite(log, string);
      fclose(log);

      return 1;
  }
  ```

### Mini Oyun Örneği
- Yalnızca belirli oyuncular için mini oyunda tespiti etkinleştirin:
  ```pawn
  CMD:mini oyunu başlat(playerid, params[]) {
      CbugDetection_Player(playerid, true);
      SendClientMessage(playerid, -1, "Mini oyunda C-Bug tespiti sizin için etkinleştirildi!");

      return 1;
  }

  CMD:mini oyunu bitir(playerid, params[]) {
      CbugDetection_Player(playerid, false);
      SendClientMessage(playerid, -1, "C-Bug tespiti sizin için devre dışı bırakıldı!");

      return 1;
  }
  ```

> [!TIP]
> Koşullu eylemler (örneğin uyarılar veya cezalar) öncesinde `CbugDetection_IsActive` kullanarak tespit durumunu kontrol edin.

## Olası Kullanım Alanları

**Cbug Detection** oldukça çok yönlüdür ve çeşitli bağlamlarda kullanılabilir:

- **Roleplay (RP) Sunucuları**: Savaşlarda gerçekçilik için **C-Bug** kurallarını uygulayın.
- **TDM/DM Sunucuları**: Savaşlarda denge sağlamak için **C-Bug**'u tespit edin.
- **İzleme**: Oyuncu davranışlarını analiz edin veya sık ihlal edenleri tespit edin.
- **Mini Oyunlar ve Etkinlikler**: Yalnızca belirli etkinliklerde **C-Bug**'u kontrol edin.
- **Özel Mekanikler**: Puanlama veya ceza sistemleriyle entegre edin.

> [!NOTE]
> Küresel olarak etkinleştirme/devre dışı bırakma sırasında bireysel ayarları koruma yeteneği, dinamik kurallı sunucular için idealdir.

## Özelleştirme

Include, varsayılan ayarlarla optimize edilmiş olsa da, defines veya `CbugDetection_OnDetected` callback'i özelleştirilebilir.

> [!WARNING]
> Defines veya diğer ayarları değiştirmek **önerilmez**, çünkü varsayılan değerler maksimum etkinlik için test edilmiş ve dengelenmiştir. Değişiklikler yanlış pozitiflere, hatalı tespitlere veya çalışmama durumuna neden olabilir.

### Defines Ayarları
- **Yüksek Pingli Sunucular**:
  ```pawn
  #define CBUG_D_SEQUENCE_INTERVAL 2000
  #define CBUG_D_SHOT_INTERVAL 300
  ```

- **Daha Sıkı Tespit**:
  ```pawn
  #define CBUG_D_SUSPICION_THRESHOLD 15.0
  ```

- **Özel Silahlar**:
  ```pawn
  #define CBUG_D_VALID_WEAPONS {24, 25, 27, 33, 34}
  ```

### Callback Özelleştirme
- Gelişmiş mantık ekleyin:
  ```pawn
  public CbugDetection_OnDetected(playerid) {
      new string[128], name[MAX_PLAYER_NAME];
      static warnings[MAX_PLAYERS];

      GetPlayerName(playerid, name, sizeof(name));
      warnings[playerid]++;

      format(string, sizeof(string), "Uyarı %d: Oyuncu %s (ID: %d) C-Bug kullanırken tespit edildi!", warnings[playerid], name, playerid);
      SendClientMessageToAll(-1, string);

      if (warnings[playerid] >= 3)
          BanEx(playerid, "Tekrarlanan C-Bug kullanımı");
      
      return 1;
  }
  ```

## Test ve Doğrulama

Doğru çalıştığından emin olmak için aşağıdaki testleri gerçekleştirin:

- **C-Bug Tespiti**:
  - Listelenen tüm varyantları izlenen silahlarla test edin.
  - `CbugDetection_OnDetected`'ın **2**-**3** dizi sonrası tetiklendiğini doğrulayın.

- **Yanlış Pozitifler**:
  - Koşma, zıplama veya çömelmeden ateş etme gibi eylemleri gerçekleştirin.
  - **C-Bug** ile ilgisiz eylemler için tespit olmadığını doğrulayın.

- **Oyuncu Bazında Kontrol**:
  - `CbugDetection_Player` ile oyuncu bazında etkinleştirme/devre dışı bırakmayı test edin.
  - Tespitin yalnızca `cbug_d_player_status` aktif oyuncular için gerçekleştiğini doğrulayın.

- **Durum Kontrolü**:
  - `CbugDetection_IsActive` ile oyuncu durumunu doğrulayın.

- **OnPlayerConnect**:
  - Yeni oyuncuların tespit devre dışı (`cbug_d_player_status = false`) olarak başladığını doğrulayın.

- **Yüksek Ping**:
  - **200**-**300ms** ping'i simüle edin ve tespit hassasiyetini kontrol edin.

- **Performans**:
  - **50**+ oyuncuyla test ederek verimliliği doğrulayın.

> [!TIP]
> Yoğun sunucularda dağıtmadan önce az sayıda oyuncuyla kontrollü bir ortamda test edin.

## Sınırlamalar ve Dikkat Edilmesi Gerekenler

- **Callback Bağımlılığı**:
  - `OnPlayerWeaponShot` ve `OnPlayerKeyStateChange`'e bağlıdır, bu da lag veya diğer include'lerden etkilenebilir.
- **Aşırı Ping**:
  - Çok yüksek gecikmeler (>**500ms**) `CBUG_D_PING_MULTIPLIER` ayarlamaları gerektirebilir.
- **Animasyonlar**:
  - Koşma/zıplama tespiti animasyon indekslerine dayanır ve değiştirilmiş istemcilerde farklılık gösterebilir.
- **Yanlış Pozitifler**:
  - Sağlam olmasına rağmen, **nadir durumlarda yanlış pozitifler** olabilir, örneğin hızlı ardışık yasal eylemlerde, bu normal kabul edilir.

> [!IMPORTANT]
> **Yanlış pozitifler**, doğrulamalarla en aza indirilmiştir, ancak alışılmadık senaryolarda meydana gelebilir. Gerekirse tespitleri izleyerek ayar yapın.

## Lisans

Bu include Apache 2.0 Lisansı altında korunmaktadır ve şunlara izin verir:

- ✔️ Ticari ve özel kullanım
- ✔️ Kaynak kodunda değişiklik
- ✔️ Kod dağıtımı
- ✔️ Patent hakları

### Koşullar:

- Telif hakkı bildirimini korumak
- Önemli değişiklikleri belgelemek
- Apache 2.0 lisansının bir kopyasını dahil etmek

Lisans hakkında daha fazla bilgi için: http://www.apache.org/licenses/LICENSE-2.0

**Telif Hakkı (c) Calasans - Tüm hakları saklıdır**