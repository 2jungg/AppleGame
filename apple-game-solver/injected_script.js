class GameStartMonitor {
    constructor() {
        this.isEnabled = true;
        this.currentGameState = null;
        this.gameStateCheckInterval = null;

        this.setupPollingMonitoring();
        this.initMessageListener();
    }

    setupPollingMonitoring() {
        this.gameStateCheckInterval = setInterval(() => {
            this.checkGameState();
        }, 1000);
    }

    initMessageListener() {
        window.addEventListener('message', (event) => {
            if (event.source !== window) return;
            
            if (event.data.type === 'APPLE_SOLVER_ENABLED') {
                this.isEnabled = event.data.enabled;
            }
        });
    }

    checkGameState() {
        const hasGame = window.exportRoot && window.exportRoot.mm;
        if (hasGame && !this.currentGameState && this.isEnabled) {
            this.onGameStart();
        } else if (hasGame && this.currentGameState === 'activate') {
-           this.onGameEnd();
        } else if (!hasGame || !this.isEnabled) {
            this.currentGameState = null;
        }
    }

    onGameStart() {
        if (this.currentGameState) return; // 이미 시작됨
        
        console.log('[Apple Game Solver] 게임 시작 감지됨. 문제 풀이 시작...');
        this.currentGameState = 'active';

        // 맵 데이터 수집 시작
        this.startMapDataCollection();
    }

    onGameEnd() {
        if (this.currentGameState === 'solved') return; // 이미 종료됨
        
        console.log('[Apple Game Solver] 문제 풀이 종료');
        this.currentGameState = 'solved';
    }

    startMapDataCollection() {
        if (!this.isEnabled) return;

        console.log('[Apple Game Solver] 맵 데이터 수집 시작...');
        
        this.collectAndSendMapData();
    }

    collectAndSendMapData() {
        try {
            const mapData = this.getMapData();
            if (mapData) {
                window.postMessage({
                    type: 'APPLE_MAP_DATA',
                    data: mapData
                }, '*');
                console.log('[Apple Game Solver] 맵 데이터 전송됨');
            }
        } catch (error) {
            console.error('[Apple Game Solver] 맵 데이터 수집 오류:', error);
        }
    }

    getMapData() {
        try {
            if (!window.exportRoot || !window.exportRoot.mm) {
                return null;
            }

            const mg = window.exportRoot.mm.mg; 
            const nuMbX = 17;
            const nuMbY = 10;
            
            let kk = 0;
            let mapData = [];
            for (let ii = 0; ii < nuMbY; ii++) {
                let row = [];
                for (let jj = 0; jj < nuMbX; jj++) {
                    kk++;
                    const cell = mg['mk' + kk];
                    
                    row.push(cell ? {
                        x: cell.x,
                        y: cell.y,
                        num: cell.nu
                    } : {
                        x: 0, 
                        y: 0, 
                        num: 0 
                    });
                }
                mapData.push(row);
            }
            return mapData;
        } catch (error) {
            console.error('[Apple Game Solver] getMapData 오류:', error);
            return null;
        }
    }

    stopMonitoring() {
        this.isEnabled = false;
        
        if (this.gameStateCheckInterval) {
            clearInterval(this.gameStateCheckInterval);
            this.gameStateCheckInterval = null;
        }
        
        console.log('[Apple Game Solver] 모니터링 중지됨');
    }

    startMonitoring() {
        this.isEnabled = true;
        this.checkGameState();
    }
}

const gameMonitor = new GameStartMonitor();

// 필요시 중지/재시작
// gameMonitor.stopMonitoring();
// gameMonitor.startMonitoring();