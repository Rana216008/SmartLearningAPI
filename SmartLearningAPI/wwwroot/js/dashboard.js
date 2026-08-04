let performanceChart;
let activityChart;

/* تحديث وضع التشغيل أو التصنيف بشكل تفاعلي */
function updateMode(type, value, clickedButton) {
    fetch(`/Dashboard/UpdateSettings?type=${type}&value=${value}`, {
        method: 'POST'
    })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'Success' || response.ok) {
                // Remove active from all buttons in the same card
                const card = clickedButton.closest('.card');
                card.querySelectorAll('.ctrl-btn').forEach(btn => btn.classList.remove('active'));
                // Add active to the clicked button
                clickedButton.classList.add('active');
                // Show success toast
                showToast('تم تحديث الإعدادات بنجاح ✓');
            } else {
                console.error('Update failed');
            }
        })
        .catch(error => console.error('Error:', error));
}

/* Toast notification */
function showToast(message) {
    let toast = document.getElementById('saveToast');
    if (!toast) {
        toast = document.createElement('div');
        toast.id = 'saveToast';
        toast.style.cssText = `
            position: fixed; bottom: 2rem; left: 50%; transform: translateX(-50%);
            background: #00e5ff; color: #000; padding: 0.8rem 2rem; border-radius: 3rem;
            font-weight: bold; z-index: 9999; opacity: 0; transition: opacity 0.3s;
        `;
        document.body.appendChild(toast);
    }
    toast.textContent = message;
    toast.style.opacity = '1';
    setTimeout(() => toast.style.opacity = '0', 2500);
}

/* Help modal */
function openHelpModal() {
    document.getElementById('helpModal').style.display = 'flex';
}
function closeHelpModal() {
    document.getElementById('helpModal').style.display = 'none';
}