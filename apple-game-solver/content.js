let isEnabled = false;
let wasmModule = null;
let wasmReady = false;
const height = 10;
const width = 17;

async function loadWasmModule() {
    try {
        const moduleLoader = await import("./app.js");
        wasmModule = await moduleLoader.default();
        wasmReady = true;
    } catch (err) {
        console.error("[Apple Game Solver] WASM 모듈 로드 실패:", err);
        wasmReady = false;
        wasmModule = null;
    }
}

function startSolver() {
    const scriptElement = document.createElement("script");
    scriptElement.src = chrome.runtime.getURL("injected_script.js");
    (document.head || document.documentElement).appendChild(scriptElement);
}

function loadAndSendInitialSettings() {
    chrome.storage.local.get(["appleGameSolverEnabled"], (result) => {
        isEnabled = result.appleGameSolverEnabled;
        sendEnabledStatus();
    });
}

function sendEnabledStatus() {
    window.postMessage(
        {
            type: "APPLE_SOLVER_ENABLED",
            enabled: isEnabled,
        },
        "*"
    );
}

startSolver();
loadWasmModule();
loadAndSendInitialSettings();

chrome.storage.onChanged.addListener((changes, areaName) => {
    if (areaName === "local" && changes.appleGameSolverEnabled) {
        isEnabled = changes.appleGameSolverEnabled.newValue;
        sendEnabledStatus();
    }
});

window.addEventListener("message", (event) => {
    if (
        event.source === window &&
        event.data &&
        event.data.type === "APPLE_MAP_DATA"
    ) {
        console.log(
            "[Apple Game Solver] 'APPLE_MAP_DATA' 수신 성공",
            event.data.data
        );

        const flatMapData = new Int32Array(height * width);
        let index = 0;
        for (let i = 0; i < height; i++) {
            for (let j = 0; j < width; j++) {
                flatMapData[index++] = event.data.data[i][j].num;
            }
        }
        try {
            const resultDragData = execWasmSolver(flatMapData);
            if (resultDragData) {
                console.log(
                    "[Apple Game Solver] 문제 풀이 결과 드래그 수:",
                    resultDragData.size
                );
                dragOperation(event.data.data, resultDragData);
            }
        } catch (error) {
            console.error("[Apple Game Solver] WASM 함수 실행 중 오류:", error);
        }
    }
});

function execWasmSolver(flatMapData) {
    if (!wasmReady || !wasmModule) {
        console.error("WASM 모듈이 로드되지 않았습니다.");
        return;
    }
    try {
        const dataSize = flatMapData.length;
        dataPtr = wasmModule._malloc(dataSize * 4);

        for (let i = 0; i < dataSize; i++) {
            wasmModule.setValue(dataPtr + i * 4, flatMapData[i], "i32");
        }

        const resultPtr = wasmModule.ccall(
            "solveMap",
            "number",
            ["number"],
            [dataPtr]
        );

        if (!resultPtr) {
            console.error("solveMap이 null 반환");
            return null;
        }

        const dragNum = wasmModule.getValue(resultPtr, "i32");
        const resultDragData = [];
        for (let i = 0; i < dragNum; i++) {
            const dragData = [
                wasmModule.getValue(resultPtr + 4 + i * 16, "i32"),
                wasmModule.getValue(resultPtr + 4 + i * 16 + 4, "i32"),
                wasmModule.getValue(resultPtr + 4 + i * 16 + 8, "i32"),
                wasmModule.getValue(resultPtr + 4 + i * 16 + 12, "i32"),
            ];
            resultDragData.push(dragData);
        }

        wasmModule._free(resultPtr);
        return resultDragData;
    } catch (error) {
        console.error("WASM 함수 실행 오류:", error);
        return null;
    } finally {
        if (dataPtr) wasmModule._free(dataPtr);
    }
}

function dragOperation(mapData, dragData) {
    const rect = document.getElementById('canvas').getBoundingClientRect();
    const offsetX = rect.left;
    const offsetY = rect.top;

    dragData.forEach(([i1, j1, i2, j2]) => {
        const startCell = mapData[i1][j1];
        const endCell = mapData[i2][j2];

        const startX = startCell.x + offsetX - 5;
        const startY = startCell.y + offsetY - 5;
        const endX = endCell.x + offsetX + 5;
        const endY = endCell.y + offsetY + 5;
        console.log(`Drag ${i1}, ${j1} to ${i2}, ${j2}`);
        console.log(`Drag ${startX}, ${startY} to ${endX}, ${endY}`);

        // 마우스 다운
        const mouseDownEvent = new MouseEvent("mousedown", {
            clientX: startX,
            clientY: startY,
            bubbles: true,
        });
        document
            .elementFromPoint(startX, startY)
            ?.dispatchEvent(mouseDownEvent);

        // 마우스 무브
        const mouseMoveEvent = new MouseEvent("mousemove", {
            clientX: endX,
            clientY: endY,
            bubbles: true,
        });
        document.elementFromPoint(endX, endY)?.dispatchEvent(mouseMoveEvent);

        // 마우스 업
        const mouseUpEvent = new MouseEvent("mouseup", {
            clientX: endX,
            clientY: endY,
            bubbles: true,
        });
        document.elementFromPoint(endX, endY)?.dispatchEvent(mouseUpEvent);
    });
}
