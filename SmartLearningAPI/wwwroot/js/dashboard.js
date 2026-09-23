let performanceChart;
let activityChart;

/* تحديث وضع التشغيل أو التصنيف بشكل تفاعلي */
function updateMode(type, value, clickedButton) {
    // تجهيز البيانات لترسل ضمن body الخاص بالطلب كـ Form Formats لتطابق الـ Action Parameters
    const formData = new URLSearchParams();
    formData.append('type', type);
    formData.append('value', value);

    fetch('/Dashboard/UpdateSettings', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: formData
    })
        .then(async response => {
            if (!response.ok) {
                throw new Error(`Server returned status: ${response.status}`);
            }
            return response.json();
        })
        .then(data => {
            // التحقق من حالة النجاح بغض النظر عن حالة الأحرف (Status / status)
            const isSuccess = data && (data.status === 'Success' || data.Status === 'Success');

            if (isSuccess) {
                // إزالة التحديد النشط من جميع الأزرار داخل الكارت الحالي
                if (clickedButton) {
                    const card = clickedButton.closest('.card');
                    if (card) {
                        card.querySelectorAll('.ctrl-btn').forEach(btn => btn.classList.remove('active'));
                    }
                    // إضافة التحديد النشط للزر المضروب عليه
                    clickedButton.classList.add('active');
                }

                // إذا كان الرد يحوي رسالة مخصصة (مثل اختيار كارت الاختبار) يتم عرضها، وإلا يظهر التنبيه الافتراضي
                const message = data.message || data.Message || 'تم تحديث الإعدادات بنجاح ✓';
                showToast(message);

                // طباعة بيانات المسار المطلوبة في وضع الاختبار للتأكد منها في Console
                if (data.askTrack || data.AskTrack) {
                    console.log(`Exam Target Track Number: ${data.askTrack || data.AskTrack}`);
                }
            } else {
                console.error('Update failed:', data);
                showToast('حدث خطأ أثناء تحديث الإعدادات ❌');
            }
        })
        .catch(error => {
            console.error('Error:', error);
            showToast('تعذر الاتصال بالخادم ❌');
        });
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
            box-shadow: 0 4px 15px rgba(0, 229, 255, 0.4);
        `;
        document.body.appendChild(toast);
    }
    toast.textContent = message;
    toast.style.opacity = '1';

    // إلغاء أي مؤقت قديم إذا تم الضغط أكثر من مرة بشكل متتالي
    if (toast.timeoutId) clearTimeout(toast.timeoutId);

    toast.timeoutId = setTimeout(() => {
        toast.style.opacity = '0';
    }, 3000);
}

/* Help modal */
function openHelpModal() {
    const modal = document.getElementById('helpModal');
    if (modal) modal.style.display = 'flex';
}

function closeHelpModal() {
    const modal = document.getElementById('helpModal');
    if (modal) modal.style.display = 'none';
}