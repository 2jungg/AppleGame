console.log('[Apple Game Solver] content script 로드됨');

let isEnabled = false;
let wasmModule = null;

chrome.storage.local.get('appleGameSolverEnabled', (data) => {
    isEnabled = data.appleGameSolverEnabled || false;

    if (isEnabled) {
        console.log("[Apple Game Solver] Auto-solver is ENABLED. Waiting for start...");
        startSolver();
    } else {
        console.log("[Apple Game Solver] Auto-solver is DISABLED.");
        stopSolver();
    }
});

chrome.storage.onChanged.addListener((changes, area) => {
    if (area === 'local' && changes.appleGameSolverEnabled) {
        isEnabled = changes.appleGameSolverEnabled.newValue;
        if (isEnabled) {
            console.log("[Apple Game Solver] Auto-solver is ENABLED. Waiting for start...");
            startSolver();
        } else {
            console.log("[Apple Game Solver] Auto-solver is DISABLED.");
            stopSolver();
        }
    }
});

window.addEventListener('message', (event) => {
    if (event.source === window && event.data && event.data.type === 'APPLE_MAP_DATA') {
        console.log("[Apple Game Solver] 'APPLE_MAP_DATA' 수신 성공", event.data.data);
    }
});

function startSolver() {
    const scriptElement = document.createElement('script');
    scriptElement.src = chrome.runtime.getURL('injected_script.js');
    (document.head || document.documentElement).appendChild(scriptElement);
    scriptElement.onload = () => {
        scriptElement.remove();
        console.log('MapData Scraper 스크립트가 성공적으로 삽입되었습니다.');
    };
}

function stopSolver() {
    window.postMessage({
        type: 'UPDATE_IS_ENABLED',
        data: {
            enabled: false
        }
    }, '*');
}

function execWasmFunction(funcName, ...args) {
    if (!wasmModule) {
        console.error("WASM 모듈이 로드되지 않았습니다.");
        return;
    }
    if (typeof wasmModule[funcName] !== 'function') {
        console.error(`WASM 함수 ${funcName}가 존재하지 않습니다.`);
        return;
    }
    return wasmModule[funcName](...args);
}
