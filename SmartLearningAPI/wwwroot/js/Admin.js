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
function editCard(id, name, uid, catId, track) {
    // 1. الوصول للقسم وإظهاره
    const section = document.getElementById('add-card-section');
    section.style.display = "block";

    // 2. تعبئة الحقول بالقيم الممرة
    document.getElementById('cardId').value = id;
    document.getElementById('cardName').value = name;
    document.getElementById('cardUID').value = uid;
    document.getElementById('cardCat').value = catId;
    document.getElementById('cardTrack').value = track;

    // 3. التحرك بسلاسة لمكان الفورم
    section.scrollIntoView({ behavior: 'smooth' });
}

/**
 * إعادة تعيين الحقول للقيم الافتراضية
 */
function resetForm() {
    const formFields = ['cardId', 'cardName', 'cardUID', 'cardTrack'];
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