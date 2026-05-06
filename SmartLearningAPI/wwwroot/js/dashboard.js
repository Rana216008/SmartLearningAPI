let performanceChart;
let activityChart;

/* تحديث وضع التشغيل أو التصنيف */
function updateMode(type, value) {
    fetch(`/Dashboard/UpdateSettings?type=${type}&value=${value}`, {
        method: 'POST'
    })
        .then(response => {
            if (response.ok) {
                location.reload();
            } else {
                console.error('Failed to update mode.');
            }
        })
        .catch(error => {
            console.error('Error:', error);
        });
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

/* إغلاق عند الضغط خارج النافذة */
window.onclick = function (event) {
    const modal = document.getElementById('statsModal');
    if (event.target === modal) {
        closeStatsModal();
    }
};

/* جلب البيانات من السيرفر */
async function fetchStats() {
    try {
        const response = await fetch('/Dashboard/GetStats');
        const data = await response.json();

        updateCharts(data);
    } catch (error) {
        console.error('Stats error:', error);
    }
}

/* تحديث الرسوم */
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
                        labels: {
                            color: '#fff'
                        }
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
                        labels: {
                            color: '#fff'
                        }
                    }
                }
            }
        });
    }
}