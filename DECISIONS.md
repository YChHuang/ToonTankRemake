

## 為什麼用 Timer 而不是 Tick?

- 日期: 2025-10-15
- 決策: 使用 GetWorldTimerManager().SetTimer()
- 原因:
  1. 生成間隔固定,不需要每幀檢查
  2. 節省效能 (Tick 每幀都執行)
  3. 程式碼更清楚 (明確的時間間隔)
- 替代方案: 在 Tick 中累計時間
- 為什麼不用: 浪費效能,邏輯更複雜

## 為什麼用 Delegate 而不是直接呼叫?

- 日期: 2025-10-16
- 決策: 使用 Multicast Delegate
- 原因:
  1. 解耦合 (SpawnManager 不需要知道 GameMode)
  2. 擴充性 (未來可能有 UI、音效等監聽者)
  3. 符合 UE5 慣例
