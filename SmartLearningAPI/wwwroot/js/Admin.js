/**
 * وظيفة إظهار وإخفاء قسم إضافة/تعديل الكروت
 */
function toggleAddCard() {
    const section = document.getElementById('add-card-section');

    // فحص حالة العرض الحالية
    if (section.style.display === "none" || section.style.display === "") {
        section.style.display = "block";
        section.scrollIntoView({ behavior: 'smooth' });
    } else {
        section.style.display = "none";
        resetForm(); // تصفير البيانات عند الإغلاق
    }
}

/**
 * وظيفة ملء بيانات الكرت في الفورم للتعديل
 */
function editCard(id, name, uid, categoryId, imageName, trackNumber, quizTrackNumber) {

    const section = document.getElementById('add-card-section');
    if (section) section.style.display = 'block';

    // تعبئة البيانات القديمة في الحقول
    document.getElementById('cardId').value = id;
    document.getElementById('cardName').value = name;
    document.getElementById('cardUID').value = uid;
    document.getElementById('cardCat').value = categoryId;

    // إسناد اسم الصورة للخانة الجديدة
    document.getElementById('cardImageName').value = imageName || '';

    document.getElementById('cardTrack').value = trackNumber;
    document.getElementById('cardQuizTrack').value = quizTrackNumber;

    // تمرير الشاشة بسلاسة نحو قسم التعديل
    section.scrollIntoView({ behavior: 'smooth' });
}

/**
 * إعادة تعيين الحقول للقيم الافتراضية
 */
function resetForm() {
    const formFields = ['cardId', 'cardName', 'cardUID', 'cardImageName', 'cardTrack', 'cardQuizTrack'];
    formFields.forEach(field => {
        const element = document.getElementById(field);
        if (element) {
            element.value = (field === 'cardId') ? "0" : "";
        }
    });
}

/**
 * وظيفة التنقل السلس لأي قسم عبر الـ ID
 */
function scrollToSection(sectionId) {
    const element = document.getElementById(sectionId);
    if (element) {
        element.scrollIntoView({ behavior: 'smooth' });
    }
}

/**
 * 🚀 وظيفة التحديث الفوري للوضع والفئة بالسيرفر لحظياً
 */
async function updateSettingsRealtime() {
    const mode = document.getElementById('modeSelect')?.value || "Learning";
    const category = document.getElementById('categorySelect')?.value || "All";

    try {
        const response = await fetch('/api/learning/update-settings', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                mode: mode,
                category: category
            })
        });

        if (response.ok) {
            console.log(`[Realtime Dashboard] Updated successfully: Mode=${mode}, Category=${category}`);
        } else {
            console.error('[Realtime Dashboard] Failed to update settings');
        }
    } catch (error) {
        console.error('[Realtime Dashboard] Error connecting to server:', error);
    }
}

// ⚡ ربط الأحداث للتنفيذ الفوري فور تغيير الأم للخيار في القائمة المنسدلة
document.addEventListener('DOMContentLoaded', function () {
    const modeSelect = document.getElementById('modeSelect');
    const categorySelect = document.getElementById('categorySelect');

    if (modeSelect) {
        modeSelect.addEventListener('change', updateSettingsRealtime);
    }

    if (categorySelect) {
        categorySelect.addEventListener('change', updateSettingsRealtime);
    }
});