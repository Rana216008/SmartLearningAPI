// وظيفة تحديث الوضع (ارسال البيانات للـ Server)
function updateMode(type, value) {
    // يمكنك إضافة كود هنا لإظهار مؤشر تحميل بسيط داخل الزر

    fetch(`/Dashboard/UpdateSettings?type=${type}&value=${value}`, {
        method: 'POST'
    }).then(response => {
        if (response.ok) {
            location.reload(); // إعادة تحميل الصفحة لتحديث شكل الأزرار النشطة
        } else {
            console.error('Failed to update mode.');
            // يمكنك إضافة تنبيه للمستخدم هنا
        }
    }).catch(error => {
        console.error('Error:', error);
    });
}

// === وظائف عرض الإحصائيات (Stats Action) ===

// وظيفة لزر الإحصائيات - تقوم بفتح الـ Modal المخصص
function openStatsModalAction() {
    console.log("Opening Stats Modal...");
    openStatsModal(); // استدعاء الوظيفة الأصلية لفتح الـ Modal
}

// وظائف النافذة المنبثقة (بقيت كما هي)
function openStatsModal() {
    const modal = document.getElementById('statsModal');
    if (modal) {
        modal.style.display = 'flex';
        initCharts();
    }
}

function closeStatsModal() {
    const modal = document.getElementById('statsModal');
    if (modal) {
        modal.style.display = 'none';
    }
}

// إغلاق الـ Modal عند النقر خارجه
window.onclick = function (event) {
    const modal = document.getElementById('statsModal');
    if (event.target == modal) {
        closeStatsModal();
    }
}

function initCharts() {
    // إعداد الرسوم البيانية (بقيت كما هي مع مثال لبيانات حقيقية)
    const ctx1 = document.getElementById('performanceChart');
    if (ctx1) {
        new Chart(ctx1.getContext('2d'), {
            type: 'doughnut',
            data: {
                labels: ['مكتسب', 'قيد التعلم'],
                datasets: [{
                    // استخدم بيانات من الـ ViewModel إذا كانت متاحة، هنا مثال ثابت
                    data: [80, 20],
                    backgroundColor: ['#00f7ff', 'rgba(255,255,255,0.1)'],
                    borderWidth: 0
                }]
            },
            options: {
                plugins: {
                    legend: {
                        position: 'bottom',
                        labels: {
                            color: '#fff',
                            font: { size: 12 }
                        }
                    }
                }
            }
        });
    }

    // مثال لرسم بياني ثانٍ (نشاط الطفل)
    const ctx2 = document.getElementById('activityChart');
    if (ctx2) {
        new Chart(ctx2.getContext('2d'), {
            type: 'bar',
            data: {
                labels: ['الأحد', 'الاثنين', 'الثلاثاء', 'الأربعاء', 'الخميس'],
                datasets: [{
                    label: 'وقت التعلم (دقيقة)',
                    data: [30, 45, 20, 60, 40],
                    backgroundColor: 'rgba(0, 247, 255, 0.5)',
                    borderColor: '#00f7ff',
                    borderWidth: 1
                }]
            },
            options: {
                scales: {
                    y: {
                        beginAtZero: true,
                        ticks: { color: '#fff' }
                    },
                    x: {
                        ticks: { color: '#fff' }
                    }
                },
                plugins: {
                    legend: {
                        labels: { color: '#fff' }
                    }
                }
            }
        });
    }
}