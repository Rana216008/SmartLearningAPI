/**
 * ==========================================
 * وظائف صفحة التحليلات والإحصائيات (Analytics.js)
 * ==========================================
 */

// 1. تحديد لون الفئة حسب نوعها
function getCategoryColor(cat) {
    const colors = {
        'Arabic': '#FF4D6A',
        'English': '#38D9F5',
        'Colors': '#FFC048'
    };
    return colors[cat] || '#A0A0C0';
}

// 2. فتح النافذة المنبثقة لتفاصيل الكرت
function openCardDetail(name, category, uid, track, scanCount, isLearned) {
    const content = document.getElementById('cardDetailContent');
    if (!content) return;

    const masteryPercent = Math.min(scanCount, 10) * 10;

    content.innerHTML = `
        <h2 style="color:var(--cyan);">${name}</h2>
        <span class="badge" style="background:${getCategoryColor(category)}; display:inline-block; padding:4px 14px; border-radius:20px; margin:10px 0;">${category}</span>
        <div style="display:flex; justify-content:space-between; margin:20px 0;">
            <div><i class="fas fa-music"></i> المسار الصوتي: ${track}</div>
            <div><i class="fas fa-hand-pointer"></i> <strong>عدد المحاولات: ${scanCount}</strong></div>
        </div>
        <div style="background:rgba(255,255,255,0.1); border-radius:10px; height:8px; margin:15px 0; overflow:hidden;">
            <div style="width:${masteryPercent}%; height:100%; background:linear-gradient(90deg, var(--cyan), var(--violet));"></div>
        </div>
        <div style="font-size:1.2rem;">${isLearned ? '✅ متقن' : '🔄 قيد التعلم'}</div>
        <div style="margin-top:10px; opacity:0.7;">UID: ${uid}</div>
    `;

    const modal = document.getElementById('cardDetailModal');
    if (modal) modal.style.display = 'flex';
}

// 3. إغلاق نافذة التفاصيل
function closeCardDetail() {
    const modal = document.getElementById('cardDetailModal');
    if (modal) modal.style.display = 'none';
}

// 4. إدارة نافذة إعادة التعيين (Reset Modal)
let resetUrl = '';

function showResetModal(el) {
    resetUrl = el.getAttribute('data-reset-url');
    const modal = document.getElementById('resetConfirmModal');
    if (modal) modal.style.display = 'flex';
}

function closeResetModal() {
    const modal = document.getElementById('resetConfirmModal');
    if (modal) modal.style.display = 'none';
}

/**
 * ==========================================
 * تهيئة الأحداث (DOM & Load Listeners)
 * ==========================================
 */
document.addEventListener('DOMContentLoaded', function () {

    // ربط إغلاق modal التفاصيل عند النقر خارجه
    const cardDetailModal = document.getElementById('cardDetailModal');
    if (cardDetailModal) {
        cardDetailModal.addEventListener('click', function (e) {
            if (e.target === this) closeCardDetail();
        });
    }

    // ربط أزرار تفاصيل الكروت (.btn-detail)
    document.querySelectorAll('.btn-detail').forEach(btn => {
        btn.addEventListener('click', function () {
            const { name, category, uid, track, count, islearned } = this.dataset;
            openCardDetail(name, category, uid, parseInt(track), parseInt(count), islearned === 'true');
        });
    });

    // ربط زر تأكيد إعادة التعيين
    const confirmResetBtn = document.getElementById('confirmResetBtn');
    if (confirmResetBtn) {
        confirmResetBtn.addEventListener('click', () => {
            if (resetUrl) window.location.href = resetUrl;
        });
    }

    // ربط إغلاق modal إعادة التعيين عند النقر خارجه
    const resetConfirmModal = document.getElementById('resetConfirmModal');
    if (resetConfirmModal) {
        resetConfirmModal.addEventListener('click', function (e) {
            if (e.target === this) closeResetModal();
        });
    }
});

// تأثير الظهور التدريجي عند اكتمال تحميل الصفحة (Cinematic reveal)
window.addEventListener('load', function () {
    setTimeout(function () {
        const el = document.getElementById('mainContent');
        if (el) el.classList.add('reveal');
    }, 600);
});