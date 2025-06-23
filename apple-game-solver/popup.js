// popup.js - Apple Game Solver 팝업 제어

document.addEventListener('DOMContentLoaded', function() {
    const toggleBtn = document.getElementById('activateBtn');
    const statusEl = document.getElementById('status');
    
    let isEnabled = false;
    
    // 확장프로그램 상태 불러오기
    function loadExtensionState() {
        chrome.storage.local.get(['appleGameSolverEnabled'], function(result) {
            isEnabled = result.appleGameSolverEnabled || false;
            updateUI();
        });
    }
    
    // 확장프로그램 상태 저장하기
    function saveExtensionState(enabled) {
        chrome.storage.local.set({ appleGameSolverEnabled: enabled });
        isEnabled = enabled;
    }
    
    // UI 업데이트
    function updateUI() {
        if (isEnabled) {
            toggleBtn.textContent = 'Off';
            toggleBtn.style.background = 'rgba(255, 0, 0, 0.7)';
            statusEl.textContent = '자동 문제 풀이가 활성화되었습니다';
        } else {
            toggleBtn.textContent = 'On';
            toggleBtn.style.background = 'rgba(255, 255, 255, 0.2)';
            statusEl.textContent = '자동 문제 풀이가 비활성화되었습니다';
        }
    }
    
    // 토글 버튼 클릭 이벤트
    toggleBtn.addEventListener('click', function() {
        const newState = !isEnabled;
        saveExtensionState(newState);
        updateUI();
        
        // 상태 메시지 임시 표시
        const tempMessage = newState ? '자동 풀이 시작!' : '자동 풀이 중지!';
        statusEl.textContent = tempMessage;
        
        setTimeout(() => {
            updateUI();
        }, 1500);
    });
    
    loadExtensionState();
});