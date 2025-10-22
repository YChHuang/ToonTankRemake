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
