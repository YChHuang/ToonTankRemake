# Tank Combat System (UE5)

> 基於 UE5 的戰車戰鬥系統，從 Tutorial 出發並進行多次架構重構與功能擴充

## 🎮 專案展示
[截圖/GIF - 展示戰車移動、射擊、爬坡]
[短影片連結 - 30-60 秒]

## 📋 專案簡介
這是一個學習型專案，從 Tutorial 起步，逐步實踐：
- 輸入系統重構（Enhanced Input）
- AI 與玩家共用控制邏輯
- 敵人生成與波次系統
- 能力系統整合（GAS）
- 物理移動與地形適應

**開發時間**：2025/07 - 持續開發中  
**UE 版本**：5.x

## ✨ 技術亮點

### 1. Enhanced Input System 整合
- 從舊版 Input 重構為 Enhanced Input
- 統一 AI Controller 與 Player Controller 的輸入接口
- **挑戰**：指標解引用導致崩潰 → 加入 nullptr 檢查

### 2. 解耦合的生成系統
- 使用 Multicast Delegate + 工廠模式
- 動態掃描地面高度避免碰撞
- **重構歷程**：從義大利麵代碼重構為符合 SOLID 原則

### 3. 物理移動系統
- 自訂 Movement Component 處理爬坡
- 實現 Pitch Align Slope（車身隨坡度旋轉）- 仍有bug
- 解決 Spring Arm 相對/世界座標轉換問題

### 4. Gameplay Ability System (GAS)
- 整合 ASC、AttributeSet、Gameplay Ability
- 實作雷射武器（Line Trace），TODO:將與原本的projectile結合，可以切換
- Cost 系統設定（彈藥消耗）

## 🛠️ 技術棧
- **引擎**：Unreal Engine 5.x
- **語言**：C++ + Blueprint
- **核心系統**：
  - Enhanced Input System
  - Gameplay Ability System
  - Custom Movement Component
  - AI Controller + Behavior Tree (規劃中)

## 📂 專案結構
```
/Content
  /Blueprints
    /Pawns   # Tank BP、AI Controller
    /Abilities    # GAS 相關 BP
    /Spawning     # 生成系統
  /Maps
/Source
  /TankCombat
    /Characters   # Pawn、Controller
    /Components   # Movement、Health
    /Abilities    # GAS C++ 實作
```

## 🎯 學習成果
- ✅ 理解 UE5 的 Delegate 機制與解耦合設計
- ✅ 掌握 GAS 基本流程與整合
- ✅ 實踐重構與架構改進（從單一責任到模組化）
- ✅ 養成寫註解與開發日誌的習慣

## 🚧 已知問題與改進方向
- [ ] Health Component 尚未遷移到 GAS AttributeSet
- [ ] 武器系統計畫改成切換系統
- [ ] AI 決策邏輯待擴充（Behavior Tree）
- [ ] 爬坡時SpringArm會亂轉

## 📖 開發日誌
詳細的技術決策、問題排查與學習筆記請見 [DEVLOG.md](DEVLOG/DECISIONS.md)

**控制方式**：
- MOUSE&KEYBOARD
- WASD：移動
- 滑鼠：瞄準/轉動砲塔
- 左鍵：射擊
- CONSOLE
- 左蘑菇頭：轉向
- RT：
- LT：

---

## 💡 關於這個專案
這是我的第一個 UE5 專案，從跟著 Tutorial(Learn C++ from scratch while making 4 fun games in Unreal Engine ) 學習到逐步加入自己的想法。  
過程中經歷多次重構，學會了如何寫出更易維護的程式碼。  

**持續更新中** | 歡迎交流與建議
