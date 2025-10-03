# UE5 Project

## 技能概述
**C++ / Unreal Engine 5**
- 熟悉語法基礎、STL 容器與演算法（`vector`, `map`, `set`, `priority_queue` 等）
- 理解記憶體管理（stack/heap、`new/delete`、智能指標）、RAII 與物件生命週期
- 掌握 OOP 與多型（`virtual/override`）、函式多載、基本模板應用
- 能獨立以 C++ 在 UE5 中實作遊戲功能（波次生成系統、遊戲結束判定、事件與計時器系統）
- 熟悉 UE 容器（`TArray`, `TMap`）、反射系統（`UCLASS`, `UPROPERTY`, `UFUNCTION`）與藍圖互動
- 具跨領域背景，能結合 3D/美術知識進行資產掛接與流程優化

---

## 專案簡介
使用 C++ 與 Unreal Engine 5 開發的小型沙盒專案，原型來自  
*Learn C++ from scratch while making 4 fun games in Unreal Engine* 教學專案，並在此基礎上進行功能擴展與優化。

---

## 新增功能
- 波次生成與計時控制，並修正勝利條件
- 修改 Tower 攻擊邏輯（將攻擊邏輯抽象到 `AIController`，使玩家可操作 Tower）
- 優化 Tank（攝影機綁定、Enhanced Input System、爬坡支援、手把相容、AIController 相容等）

---

## 開發難點與解法

### Pawn 優化
**問題 1：輸入來源依賴**  
- 背景：原版炮塔瞄準依賴滑鼠游標與 `HitResult`，不支援搖桿與 AIController。  
- 解法：將 `RotateTurret(FVector LookAtTarget)` 改為 `RotateTurret(const FInputActionValue& Value)`，支援多種輸入設備。

**問題 2：操作模式限制**  
- 背景：`GetHitResultUnderCursor` 僅限滑鼠，搖桿輸入為 float 軸值。  
- 解法：改為計算 `DeltaYaw`，並引入 `DeltaSeconds` 控制旋轉速度，確保不同幀率一致。

**問題 3：AIController 擴展困難**  
- 背景：AI 無游標概念，無法直接指向玩家。  
- 解法：將旋轉邏輯抽象到 AIController，使用 `FindDeltaAngleDegrees` + `Clamp` 控制角度差。

**問題 4：砲塔旋轉過度 (overshooting)**  
- 解法：在 AIController 中對角度差 `Clamp` 限制，避免無限旋轉。

**問題 5：AIController 無法取得砲塔指標**  
- 解法：在基類或組件中提供 getter，讓 AIController 安全存取。

**問題 6：無法爬斜坡**  
- 背景：`AddActorLocalOffset` 無法處理斜坡。  
- 解法：改用 `MovementComponent` + `AddVector`。

**問題 7：無法跌落**  
- 背景：爬坡後角色漂浮。  
- 解法：用 delegate 偵測浮空，給予重力向量更新 Z 軸。

**問題 8：Tank 攝影機 UX 問題**  
- 背景：`SpringArm` 綁在 Turret，會受 pitch/roll 影響。  
- 解法：在 `Tick` 更新 `SpringArm`，僅取 yaw。

---

### Enemy Spawner
**問題 1：GameOver 提前觸發**  
- 背景：生成時 `Count++`，導致未生成完成就被計入。  
- 解法：加入 `RemainingWave` 計數，僅在波次完成且 `Count == 0` 時觸發。

**問題 2：生成位置錯誤（掉進地板）**  
- 背景：角色高度未知，碰撞檢測失敗。  
- 解法：初版用 Dummy 取高度，後改為 Blueprint 可編輯哈希表 `<UClass, 高度>`，效能更佳。

**問題 3：架構擴充困難**  
- 背景：藍圖依賴注入過多，循環依賴。  
- 解法：重構為多播委託 + 工廠模式，透過 delegate 訂閱事件，降低耦合。

**問題 4：玩家/敵人邏輯混淆**  
- 背景：Enemy 邏輯寫死在 Tower class。  
- 解法：將 Tower 抽象為基類，生成時加 Tag 區分角色，並 override 為 NPC/Player 版本。

---

## TODO
- Camera 應綁在 root，並在 `Tick` 更新 `SpringArm`，僅保留 yaw。
