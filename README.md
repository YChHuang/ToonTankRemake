# UE5porject
ue5
C++（熟悉語法基礎、STL 容器與演算法、記憶體管理與 OOP；具 UE4/UE5 專案開發經驗）
C++：
- 熟悉語法基礎、STL 容器與演算法（vector, map, set, priority_queue 等）
- 理解記憶體管理（stack/heap、new/delete、智能指標）、RAII 與物件生命週期
- 掌握 OOP 與多型（virtual/override）、函式多載、基本模板應用
- 能獨立以 C++ 在 UE5 中實作遊戲功能（波次生成系統、遊戲結束判定、事件與計時器系統）
- 熟悉 UE 容器（TArray, TMap）、反射系統（UCLASS, UPROPERTY, UFUNCTION）與藍圖互動
- 具跨領域背景，能結合 3D/美術知識進行資產掛接與流程優化




## 專案簡介
一個使用 C++ 與 Unreal Engine 5 實作的波次生成系統，包含敵人生成、波次切換、遊戲結束判定等功能。

## 功能特色
- 波次生成與計時控制（使用 UE Timer）
- 敵人數量追蹤與事件廣播（Delegate）
- 與美術資產整合，支援藍圖與 C++ 互動
- 可擴充的波次設定（TArray 配置）

## 特色更新
- 增加Enhanced Input System
- 增加Spawn System
- 將原版許多功能抽象給AiController，避面過度耦合


# 開發痛點

## Enhanced Input System 改造炮塔旋轉邏輯

### 背景
原版炮塔瞄準系統在 `Tick` 中使用滑鼠游標掃描場景，取得目標座標後傳入 `RotateTurret()` 進行平滑旋轉。此設計依賴滑鼠輸入與 `HitResult` 計算，不支援搖桿操作，且對 AIController 擴展不友好。

### 痛點

#### 問題 1：輸入來源耦合
**原因**：函式簽名直接依賴世界座標，無法抽象成 Enhanced Input 的 `InputAction` 向量，限制了輸入模式的通用性。  
**解法**：將 `RotateTurret(FVector LookAtTarget)` 改為 `RotateTurret(const FInputActionValue& Value)`，使函式可直接接收玩家輸入值，支援多種輸入設備。

---

#### 問題 2：操作模式限制，GetHitResultUnderCursor僅限滑鼠
**原因**：搖桿輸入為軸值（float），無法直接轉換成目標座標，原本的瞄準邏輯依賴滑鼠游標與世界座標。  
**解法**：移除目標座標計算，改為根據玩家輸入值計算 `DeltaYaw`，並引入 `DeltaSeconds` 控制旋轉速度，確保不同幀率下旋轉一致。

---

#### 問題 3：AI 擴展困難
**原因**：AI 無游標概念，原本 Enemy 的邏輯是直接把玩家座標套用在 `RotateTurret()`，但改成 float 輸入後無法直接指向玩家。  
**解法**：將旋轉邏輯抽象到 AIController，使用 `FindDeltaAngleDegrees`（底層 `atan2` 計算弧度再轉成 float）取得角度差，並透過 `Clamp` 限制旋轉範圍，動態調整兩端避免 overshooting。

---

#### 問題 4：砲塔旋轉過度（overshooting）
**原因**：AIController 呼叫 `RotateTurret()` 時輸入 float，缺乏角度差限制，導致砲塔無限轉圈。  
**解法**：在 AIController 中使用 `Clamp` 對角度差進行限制，並根據需求動態調整最大最小值，防止過度旋轉。

---

#### 問題 5：AIController 無法取得砲塔指標
**原因**：當時沒有 getter 的概念，嘗試直接存取砲塔指標失敗。  
**解法**：在基類或相關組件中提供明確的 getter 方法，讓 AIController 能安全取得砲塔指標並進行控制。

### 成果
此改動使炮塔旋轉邏輯同時支援滑鼠與搖桿輸入，並為 AIController 提供更通用的控制接口，降低後續擴展成本。

---

## Spawn & Wave System

### 背景
原版無生成系統，依賴的是 Editor 上預先設置好的 ActorPawn 來處理遊戲規則邏輯。我想開發一個能依照波次生成，且可以選擇要生成的派生類，並與當前的 GameMode 規則配合。當前勝利邏輯為開場統計 EnemyCount，並在 EnemyDied 時減少計數，當計數為 0 → GameOver。

### 痛點

#### 問題 1：GameOver 提前觸發
**原因**：若使用生成後將原本的 Count++，生成的敵人尚未完全生成就被計入 EnemyCount，導致計數歸零過早觸發。  
**解法**：加入 RemainingWave 計數，僅在波次完成且 Count == 0 後才觸發 GameOver 條件。

---

#### 問題 2：生成位置錯誤（敵人掉進地板）
**原因**：生成時碰撞檢測仍須角色高度的 offset，角色高度未知，導致一系列障礙。  
**解法**：初版用 Spawn 一個 DummyVersion 取 CapsuleHeight 再刪掉 dummy，後改為 Blueprint 可編輯的哈希表儲存 `<UClass, 高度>`。原版雖然較精準但效能浪費，查表法更高效。

---

#### 問題 3：架構擴充困難
**原因**：藍圖依賴注入過多、循環依賴，後續增加新 spawner/EnemyClass 困難度極高。  
**解法**：重構為多播委託 + 工廠模式生成 spawnerManager，並透過 delegate 訂閱事件，減少依賴注入。

---

#### 問題 4：玩家/敵人邏輯混淆
**原因**：原版本 Enemy 邏輯寫死在 Tower class，泛化後玩家死亡邏輯與 Enemy 衝突。  
**解法**：將 Tower 降維成基類，生成時加 Tag 區分角色，並 override 基類方法為 NPCver / PlayerVer。
