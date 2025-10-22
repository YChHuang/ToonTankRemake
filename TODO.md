# TODO List

## 🔥 高優先級 (Critical)

### SpawnManager 重構
- [ ] 修改 GameMode,在 BeginPlay 中建立 SpawnManager (而非每次 StartWave)
- [ ] 統一變數命名規範 (CurrentWave, EnemyRemainCount 等)
- [ ] 修正 WaveIndex 驗證邏輯 (`>` 改為 `>=`)
- [ ] 將 magic number (2.0f) 改為 UPROPERTY(SpawnInterval)
- [ ] 加上 SpawnManager 的類別註解說明

### 錯誤處理
- [ ] 增加 SpawnManager null check
- [ ] 處理 Spawner 生成失敗的情況
- [ ] 加上 WaveConfigs 空陣列檢查

---

## 📋 中優先級 (Important)

### 程式碼品質
- [ ] 為所有 public 函數加上註解
- [ ] 用區塊註解 (`=== XXX ===`) 組織程式碼邏輯
- [ ] 檢查所有 UPROPERTY 是否正確使用 EditDefaultsOnly/EditAnywhere
- [ ] 統一 Log 分類 (考慮建立自訂 LogCategory)

### 功能改進
- [ ] 實作 StopWave() 功能
- [ ] 加上波次結束事件 (OnWaveComplete)
- [ ] 考慮加入暫停/恢復波次功能
- [ ] 實作不同敵人類型的生成邏輯 (目前 TODO 註解提到但未實作)

### 測試
- [ ] 測試沒有 Spawner 的情況
- [ ] 測試 WaveConfigs 超出範圍
- [ ] 測試連續快速呼叫 StartWave()
- [ ] 測試 SpawnManager 被提前銷毀的情況

---

## 💡 低優先級 (Nice to Have)

### 效能優化
- [ ] 考慮用 Object Pool 管理敵人生成
- [ ] 避免每次都用 GetAllActorsOfClass (可以在 Level 載入時快取)

### 可維護性
- [ ] 建立 ARCHITECTURE.md 說明系統架構
- [ ] 考慮將 Wave 配置移到 DataTable 或 DataAsset
- [ ] 加上單元測試 (Automation Test)

### 擴充性
- [ ] 支援多種敵人類型配置
- [ ] 支援特殊波次 (Boss Wave, Bonus Wave)
- [ ] 加入難度曲線設定 (敵人生命值/傷害隨波次增加)

---

## ✅ 已完成

- [x] 基礎波次系統
- [x] 隨機 Spawner 選擇
- [x] Delegate 事件系統
- [x] 基本錯誤檢查

---

## 📝 筆記

### 設計決策
- SpawnManager 使用 Timer 而非 Tick (節省效能)
- 採用 Delegate 而非直接引用 (降低耦合)
- 使用 TArray 而非 Map 存 Wave 配置 (簡單直觀)

### 已知問題
- SpawnManager 目前是動態生成,需要改為單例模式
- 沒有處理波次中途更換關卡的情況
- Spawner 生成失敗時不會重試

### 未來考慮
- 是否需要加入生成預告 (提前告知玩家即將生成)
- 是否需要加入生成特效/音效
- 是否需要支援多個 SpawnManager (不同區域獨立波次)

---

## 🔗 相關文件

- [Code Review](./Docs/CODE_REVIEW.md) - SpawnManager 程式碼審查
- [Architecture](./Docs/ARCHITECTURE.md) - 系統架構說明
- [Changelog](./Docs/CHANGELOG.md) - 版本變更記錄

---

## 📅 更新日誌

**2025-10-22**
- 初始化 TODO 清單
- 新增 SpawnManager 重構任務

```
2. 變數命名不一致
cppcurrentWave     // 小寫開頭
EnemyRemainCount // 大寫開頭
WaveIndex       // 參數大寫
waveConfig      // 成員變數小寫
問題: 難以區分變數類型和作用域
建議: 統一命名規範:
cpp// 成員變數
int32 CurrentWave;
int32 EnemyRemainCount;
TArray<int32> WaveConfigs;

// 參數
void StartWave(int32 InWaveIndex)

// 區域變數
int32 randomIndex = ...;
3. 缺少註解和意圖說明
cppvoid ASpawnManager::handleWave()
{
    // 這個函數到底在做什麼?為什麼叫 handleWave?
問題: 3 個月後完全忘記邏輯
4. 錯誤處理不完整
cppif (WaveIndex > waveConfig.Num())  // 應該是 >= ?
{
    return;  // 返回後沒有清理
}
5. magic number
cpp2.0f,  // 為什麼是 2 秒?
```
