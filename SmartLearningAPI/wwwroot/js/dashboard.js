let performanceChart;
let activityChart;

/* تحديث وضع التشغيل أو التصنيف بشكل تفاعلي سريع */
function updateMode(type, value, clickedButton) {
    // إرسال الطلب للسيرفر في الخلفية دون تعطيل الصفحة
    fetch(`/Dashboard/UpdateSettings?type=${type}&value=${value}`, {
        method: 'POST'
    })
        .then(response => {
            if (response.ok) {
                // 1. العثور على الحاوية الخاصة بالقسم الحالي (إما قسم الوضع أو قسم اللغة)
                const sectionBox = clickedButton.closest('.section-box');

                // 2. إزالة حالة النشاط (active) من جميع الأزرار داخل هذا القسم فقط
                sectionBox.querySelectorAll('.glass-button').forEach(btn => {
                    btn.classList.remove('active');
                });

                // 3. إضافة حالة النشاط للزر الذي ضغطت عليه الأم فوراً
                clickedButton.classList.add('active');

                // 4. إظهار علامة الصح وإشعار النجاح للأم
                showSuccessToast();
            } else {
                console.error('Failed to update mode.');
            }
        })
        .catch(error => {
            console.error('Error:', error);
        });
}

/* دالة إظهار إشعار علامة الصح وتأكيد المزامنة */
function showSuccessToast() {
    const toast = document.getElementById('saveToast');
    if (toast) {
        toast.classList.add('show');

        // إخفاء الإشعار تلقائياً بعد ثانيتين ونصف
        setTimeout(() => {
            toast.classList.remove('show');
        }, 2500);
    }
}

/* فتح نافذة الإحصائيات */
function openStatsModalAction() {
    openStatsModal();
    fetchStats();
}

function openStatsModal() {
    const modal = document.getElementById('statsModal');
    if (modal) {
        modal.style.display = 'flex';
    }
}

/* إغلاق النافذة */
function closeStatsModal() {
    const modal = document.getElementById('statsModal');
    if (modal) {
        modal.style.display = 'none';
    }
}

window.onclick = function (event) {
    const modal = document.getElementById('statsModal');
    if (event.target === modal) {
        closeStatsModal();
    }
};

/* جلب بيانات الأداء */
async function fetchStats() {
    try {
        const response = await fetch('/Dashboard/GetStats');
        const data = await response.json();
        updateCharts(data);
    } catch (error) {
        console.error('Stats error:', error);
    }
}

/* تحديث الرسوم البيانية */
function updateCharts(data) {
    if (performanceChart) performanceChart.destroy();
    if (activityChart) activityChart.destroy();

    const ctx1 = document.getElementById('performanceChart');
    const ctx2 = document.getElementById('activityChart');

    if (ctx1) {
        performanceChart = new Chart(ctx1.getContext('2d'), {
            type: 'doughnut',
            data: {
                labels: ['مكتسب', 'قيد التعلم'],
                datasets: [{
                    data: [data.learned, data.remaining],
                    backgroundColor: ['#ffb36b', '#ff7a18'],
                    borderWidth: 0
                }]
            },
            options: {
                plugins: {
                    legend: {
                        position: 'bottom',
                        labels: { color: '#fff' }
                    }
                }
            }
        });
    }

    if (ctx2) {
        activityChart = new Chart(ctx2.getContext('2d'), {
            type: 'bar',
            data: {
                labels: data.days,
                datasets: [{
                    label: 'وقت التعلم',
                    data: data.minutes,
                    backgroundColor: '#ffb36b'
                }]
            },
            options: {
                responsive: true,
                scales: {
                    y: { beginAtZero: true, ticks: { color: '#fff' } },
                    x: { ticks: { color: '#fff' } }
                },
                plugins: {
                    legend: { labels: { color: '#fff' } }
                }
            }
        });
    }
}