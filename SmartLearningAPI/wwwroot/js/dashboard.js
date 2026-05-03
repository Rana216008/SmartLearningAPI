// Progress Chart
new Chart(document.getElementById('progressChart'), {
    type: 'doughnut',
    data: {
        labels: ['تم التعلم', 'متبقي'],
        datasets: [{
            data: [12, 16],
            backgroundColor: ['green', 'gray']
        }]
    }
});

// Result Chart
new Chart(document.getElementById('resultChart'), {
    type: 'bar',
    data: {
        labels: ['صح', 'خطأ'],
        datasets: [{
            data: [30, 10]
        }]
    }
});