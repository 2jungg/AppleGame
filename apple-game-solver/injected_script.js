let solverCheckInterval = null;
let isEnabled = true;

window.addEventListener('message', (event) => {
    if (event.source === window && event.data && event.data.type === 'UPDATE_IS_ENABLED') {
        if (!isEnabled && solverCheckInterval) {
            clearInterval(solverCheckInterval);
            solverCheckInterval = null;
            console.log('[Apple Game Solver] 문제 풀이 비활성화됨');
        } else if (isEnabled && !solverCheckInterval && (!window.exportRoot || !window.exportRoot.mm)) {
            console.log('[Apple Game Solver] 다시 게임 감지 시작...');
            startObserving();
        }
    }
});

function startObserving() {
    if (solverCheckInterval) {
        clearInterval(solverCheckInterval);
        solverCheckInterval = null; // 인터벌 ID 초기화
    }
    if (window.exportRoot && window.exportRoot.mm) {
        console.log('[Apple Game Solver] 게임 시작 감지됨. 문제 풀이 시작...');
        const mapData = getMapData();

        // 콘텐츠 스크립트로 데이터 전송
        window.postMessage({
            type: 'APPLE_MAP_DATA',
            data: mapData
        }, '*');
        return;
    }

    // 게임 객체가 아직 로드되지 않았다면 주기적으로 확인
    console.log('[Apple Game Solver] 게임 시작 감지중...');
    solverCheckInterval = setInterval(() => {
        // isEnabled가 false가 되면 중지 (사용자가 비활성화한 경우)
        if (!isEnabled) { // 주입된 스크립트 내의 isEnabled 변수 사용
            clearInterval(solverCheckInterval);
            solverCheckInterval = null;
            console.log('[Apple Game Solver] 문제 풀이 비활성화됨');
            return;
        }

        if (window.exportRoot && window.exportRoot.mm) {
            console.log('[Apple Game Solver] 게임 시작 감지됨. 문제 풀이 시작...');
            clearInterval(solverCheckInterval);
            solverCheckInterval = null;
            const mapData = getMapData();
            window.postMessage({
                type: 'APPLE_MAP_DATA',
                data: mapData
            }, '*');
            return;
        } else {
            console.log('[Apple Game Solver] 게임 시작 감지중...');
        }
    }, 500);
}

function getMapData() {
    try {
        // window.exportRoot는 웹 페이지의 전역 객체이므로 직접 접근 가능
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
                    num: cell.nu}:
                    { x: 0, y: 0, num: 0 });
            }
            mapData.push(row);
        }
        return mapData;
    } catch (error) {
        return null; // 오류 발생 시 null 반환
    }
}

startObserving();
