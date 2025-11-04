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
- Camera 應綁在 root，並在 `Tick` 更新 `SpringArm`，僅保留 yaw。//done 20251020
