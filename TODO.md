# SpawnManager Code Review

## 📋 架構概覽

你的架構設計思路不錯,流程如下:

```
GameMode (遊戲總控)
    ↓ 呼叫 StartWave()
SpawnManager (波次管理器)
    ↓ 透過 Timer 定期呼叫
Spawner (生成點) × N 個
    ↓ 生成
Enemy (敵人)
```

### 執行流程

1. GameMode 開始波次 → 動態生成 SpawnManager
2. SpawnManager 找到場景中所有 Spawner
3. 每 2 秒隨機選一個 Spawner 生成敵人
4. 透過 Delegate 回報生成事件

---

## 🔍 問題點分析

### 1. 動態生成 SpawnManager 的設計疑慮

**目前寫法:**
```cpp
SpawnManager = GetWorld()->SpawnActor<ASpawnManager>(SpawnManagerClass, ...);
```

**問題:**
- SpawnManager 是**管理者**,不應該像敵人一樣動態生成
- 每次開波次都 Spawn 一個新的,舊的怎麼辦?
- 這樣會有記憶體洩漏風險

**建議:**  
SpawnManager 應該:
- 在 GameMode 的 `BeginPlay()` 中建立一次
- 或直接作為 GameMode 的 Component
- 或放在場景中(像 GameState 一樣)

---

### 2. 變數命名不一致

**目前的命名:**
```cpp
currentWave     // 小寫開頭
EnemyRemainCount // 大寫開頭
WaveIndex       // 參數大寫
waveConfig      // 成員變數小寫
```

**問題:**  
難以區分變數類型和作用域

**建議統一命名規範:**
```cpp
// 成員變數
int32 CurrentWave;
int32 EnemyRemainCount;
TArray<int32> WaveConfigs;

// 參數
void StartWave(int32 InWaveIndex)

// 區域變數
int32 randomIndex = ...;
```

---

### 3. 缺少註解和意圖說明

**目前寫法:**
```cpp
void ASpawnManager::handleWave()
{
    // 這個函數到底在做什麼?為什麼叫 handleWave?
}
```

**問題:**  
3 個月後完全忘記邏輯

---

### 4. 錯誤處理不完整

```cpp
if (WaveIndex > waveConfig.Num())  // 應該是 >= ?
{
    return;  // 返回後沒有清理
}
```

---

### 5. Magic Number

```cpp
2.0f,  // 為什麼是 2 秒?
```

**建議:**  
改用可配置的 UPROPERTY

---

## ✅ 改進版本

### 1. GameMode 改進

#### Header (.h)

```cpp
// ToonTanksGameModeBase.h
UCLASS()
class AToonTanksGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    // === Wave Management ===
    
    /** 波次管理器 - 負責控制敵人生成 */
    UPROPERTY()
    ASpawnManager* SpawnManager;
    
    /** SpawnManager 的類別,可在 BP 中設定 */
    UPROPERTY(EditDefaultsOnly, Category = "Wave")
    TSubclassOf<ASpawnManager> SpawnManagerClass;
    
    /** 當前波次 (從 0 開始) */
    UPROPERTY()
    int32 CurrentWave;
    
    /** 當前波次剩餘敵人數量 */
    UPROPERTY()
    int32 EnemyRemainCount;

public:
    /** 開始指定波次 */
    void StartWave(int32 WaveIndex);

private:
    /** 當波次開始時的回調 - 更新剩餘敵人計數 */
    UFUNCTION()
    void HandleWaveStart(int32 WaveEnemyCount);
    
    /** 當敵人死亡時的回調 */
    UFUNCTION()
    void HandleEnemyDestroyed();
};
```

#### Implementation (.cpp)

```cpp
// ToonTanksGameModeBase.cpp
void AToonTanksGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    
    // 初始化 SpawnManager (只建立一次)
    if (SpawnManagerClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Name = FName("SpawnManager");
        
        SpawnManager = GetWorld()->SpawnActor<ASpawnManager>(
            SpawnManagerClass, 
            FVector::ZeroVector, 
            FRotator::ZeroRotator,
            SpawnParams
        );
        
        if (SpawnManager)
        {
            // 綁定事件
            SpawnManager->OnWaveStart.AddDynamic(this, &AToonTanksGameModeBase::HandleWaveStart);
            SpawnManager->OnEnemySpawned.AddDynamic(this, &AToonTanksGameModeBase::HandleEnemySpawned);
        }
    }
}

void AToonTanksGameModeBase::StartWave(int32 WaveIndex)
{
    if (!SpawnManager)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnManager is null! Cannot start wave."));
        return;
    }
    
    CurrentWave = WaveIndex;
    SpawnManager->StartWave(WaveIndex);
}

void AToonTanksGameModeBase::HandleWaveStart(int32 WaveEnemyCount)
{
    EnemyRemainCount = WaveEnemyCount;
    UE_LOG(LogTemp, Warning, TEXT("Wave %d started with %d enemies"), CurrentWave, EnemyRemainCount);
}
```

---

### 2. SpawnManager 改進

#### Header (.h)

```cpp
// SpawnManager.h

/**
 * SpawnManager - 波次生成管理器
 * 
 * 職責:
 * - 管理多個 EnemySpawner
 * - 根據波次配置定時生成敵人
 * - 追蹤生成進度並通知 GameMode
 * 
 * 使用方式:
 * 1. 在關卡中放置多個 EnemySpawner
 * 2. GameMode 呼叫 StartWave(WaveIndex)
 * 3. 透過 OnWaveStart/OnEnemySpawned 接收事件
 * 
 * 波次流程:
 * GameMode::StartWave()
 *   → SpawnManager::StartWave()
 *     → 啟動 Timer (每 SpawnInterval 秒)
 *       → SpawnEnemyTick()
 *         → 隨機選 Spawner
 *         → Spawner::SpawnEnemy()
 *           → 觸發 OnEnemySpawned
 */
UCLASS()
class ASpawnManager : public AActor
{
    GENERATED_BODY()

public:
    // === Delegates ===
    
    /** 當波次開始時觸發 (參數:敵人總數) */
    UPROPERTY(BlueprintAssignable, Category = "Wave")
    FOnWaveStartSignature OnWaveStart;
    
    /** 當敵人生成時觸發 */
    UPROPERTY(BlueprintAssignable, Category = "Wave")
    FOnEnemySpawnedSignature OnEnemySpawned;

protected:
    virtual void BeginPlay() override;

private:
    // === Wave Configuration ===
    
    /** 每波的敵人數量配置 [波次索引 -> 敵人數量] */
    UPROPERTY(EditDefaultsOnly, Category = "Wave", 
        meta = (DisplayName = "Wave Enemy Counts", 
                ToolTip = "索引對應波次,值為該波次的敵人數量"))
    TArray<int32> WaveConfigs = {5, 10, 15, 20}; // 預設值
    
    /** 生成敵人的時間間隔(秒) */
    UPROPERTY(EditDefaultsOnly, Category = "Wave", 
        meta = (ClampMin = "0.5", ClampMax = "10.0",
                ToolTip = "每隔多久生成一個敵人"))
    float SpawnInterval = 2.0f;
    
    // === Runtime State ===
    
    /** 當前波次索引 */
    int32 CurrentWaveIndex;
    
    /** 本波次還需生成的敵人數量 */
    int32 RemainingSpawnCount;
    
    /** 場景中所有的生成點 */
    UPROPERTY()
    TArray<AEnemySpawner*> SpawnerList;
    
    /** 生成計時器 */
    FTimerHandle SpawnTimerHandle;

public:
    /** 開始指定波次 */
    void StartWave(int32 WaveIndex);
    
    /** 停止當前波次 */
    void StopWave();

private:
    /** 初始化:找到場景中所有 Spawner */
    void InitializeSpawners();
    
    /** 定時器回調:執行一次敵人生成 */
    void SpawnEnemyTick();
    
    /** 當 Spawner 成功生成敵人時的回調 */
    UFUNCTION()
    void HandleEnemySpawned(AEnemy* SpawnedEnemy);
};
```

#### Implementation (.cpp)

```cpp
// SpawnManager.cpp

void ASpawnManager::BeginPlay()
{
    Super::BeginPlay();
    InitializeSpawners();
}

void ASpawnManager::InitializeSpawners()
{
    // === 找到場景中所有 EnemySpawner ===
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundActors);
    
    SpawnerList.Empty();
    for (AActor* Actor : FoundActors)
    {
        if (AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor))
        {
            SpawnerList.Add(Spawner);
            
            // 綁定生成事件
            Spawner->OnEnemySpawned.AddDynamic(this, &ASpawnManager::HandleEnemySpawned);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("SpawnManager: Found %d spawners in level"), SpawnerList.Num());
}

void ASpawnManager::StartWave(int32 WaveIndex)
{
    // === 參數驗證 ===
    if (!WaveConfigs.IsValidIndex(WaveIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid wave index: %d (max: %d)"), 
            WaveIndex, WaveConfigs.Num() - 1);
        return;
    }
    
    if (SpawnerList.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No spawners found in level! Cannot start wave."));
        return;
    }
    
    // === 初始化波次狀態 ===
    CurrentWaveIndex = WaveIndex;
    RemainingSpawnCount = WaveConfigs[WaveIndex];
    
    UE_LOG(LogTemp, Warning, TEXT("=== Wave %d Start === (Enemies: %d)"), 
        CurrentWaveIndex, RemainingSpawnCount);
    
    // 通知 GameMode
    OnWaveStart.Broadcast(RemainingSpawnCount);
    
    // === 開始生成計時器 ===
    // 每 SpawnInterval 秒生成一個敵人,直到本波次配額用完
    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &ASpawnManager::SpawnEnemyTick,
        SpawnInterval,
        true,  // 循環執行
        0.0f   // 立即開始第一次
    );
}

void ASpawnManager::StopWave()
{
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("Wave %d stopped"), CurrentWaveIndex);
}

void ASpawnManager::SpawnEnemyTick()
{
    // === 檢查是否還需要生成 ===
    if (RemainingSpawnCount <= 0)
    {
        StopWave();
        UE_LOG(LogTemp, Warning, TEXT("Wave %d: All enemies spawned"), CurrentWaveIndex);
        return;
    }
    
    // === 隨機選擇一個 Spawner ===
    int32 RandomIndex = FMath::RandRange(0, SpawnerList.Num() - 1);
    AEnemySpawner* SelectedSpawner = SpawnerList[RandomIndex];
    
    if (!SelectedSpawner)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected spawner is null!"));
        return;
    }
    
    // === 嘗試生成敵人 ===
    bool bSpawnSuccess = SelectedSpawner->SpawnEnemy();
    
    if (bSpawnSuccess)
    {
        RemainingSpawnCount--;
        UE_LOG(LogTemp, Log, TEXT("Spawned enemy at spawner %d (Remaining: %d)"), 
            RandomIndex, RemainingSpawnCount);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy at spawner %d"), RandomIndex);
    }
}

void ASpawnManager::HandleEnemySpawned(AEnemy* SpawnedEnemy)
{
    if (!SpawnedEnemy)
    {
        return;
    }
    
    // 轉發事件給 GameMode
    OnEnemySpawned.Broadcast(SpawnedEnemy);
}
```

---

## 📝 如何避免「看不懂自己的程式碼」?

### 1. 寫註解的黃金法則

```cpp
// ❌ 不好:說明「做什麼」(程式碼已經說了)
// 設定計時器
GetWorldTimerManager().SetTimer(...);

// ✅ 好:說明「為什麼」(意圖)
// 每 2 秒生成一個敵人,直到本波次配額用完
GetWorldTimerManager().SetTimer(...);
```

---

### 2. 用區塊註解組織程式碼

```cpp
void MyFunction()
{
    // === 參數驗證 ===
    if (!IsValid()) return;
    
    // === 初始化 ===
    Setup();
    
    // === 執行邏輯 ===
    DoWork();
    
    // === 清理 ===
    Cleanup();
}
```

---

### 3. 函數命名要清楚

```cpp
// ❌ 不好
void handleWave()  // handle 是什麼意思?

// ✅ 好
void SpawnEnemyTick()  // 清楚說明這是生成敵人的定時回調
```

---

### 4. 把 Magic Number 變成常數

```cpp
// ❌ 不好
SetTimer(..., 2.0f, true);

// ✅ 好
UPROPERTY(EditDefaultsOnly, Category = "Wave")
float SpawnInterval = 2.0f;

SetTimer(..., SpawnInterval, true);
```

---

### 5. 用 UPROPERTY 的 meta 提供提示

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Wave", 
    meta = (DisplayName = "每波敵人數量", 
            ToolTip = "索引對應波次,值為該波次的敵人數量"))
TArray<int32> WaveConfigs;
```

---

### 6. 在 Header 寫摘要註解

```cpp
/**
 * SpawnManager - 波次生成管理器
 * 
 * 職責:
 * - 管理多個 EnemySpawner
 * - 根據波次配置定時生成敵人
 * - 追蹤生成進度並通知 GameMode
 * 
 * 使用方式:
 * 1. 在關卡中放置多個 EnemySpawner
 * 2. GameMode 呼叫 StartWave(WaveIndex)
 * 3. 透過 OnWaveStart/OnEnemySpawned 接收事件
 */
UCLASS()
class ASpawnManager : public AActor
```

---

### 7. 用流程圖輔助

在註解中加入簡單的流程:

```cpp
/**
 * 波次流程:
 * GameMode::StartWave()
 *   → SpawnManager::StartWave()
 *     → 啟動 Timer (每 SpawnInterval 秒)
 *       → SpawnEnemyTick()
 *         → 隨機選 Spawner
 *         → Spawner::SpawnEnemy()
 *           → 觸發 OnEnemySpawned
 */
```

---

## 🎯 總結

### ✅ 程式碼優點

- 架構分層清楚 (GameMode → SpawnManager → Spawner)
- 使用 Delegate 解耦合
- 有錯誤檢查

### ❌ 需要改進

- SpawnManager 不應動態生成
- 命名不一致
- 缺少註解
- Magic Number
- 錯誤處理不完整

### 💡 改進建議

1. **在 GameMode 的 BeginPlay 建立 SpawnManager**
2. **統一命名規範**
3. **加上意圖註解和區塊註解**
4. **用 UPROPERTY 取代 Magic Number**
5. **在 .h 檔寫摘要說明**

---

## 📌 後續任務

參考 `TODO.md` 中的任務清單進行重構。
