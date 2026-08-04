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
    // إظهار السيكشن الخاص بالفورم أولاً
    const section = document.getElementById('add-card-section');
    if (section) section.style.display = 'block';

    // تعبئة البيانات القديمة في الحقول
    document.getElementById('cardId').value = id;
    document.getElementById('cardName').value = name;
    document.getElementById('cardUID').value = uid;
    document.getElementById('cardCat').value = categoryId;

    // 👇 إسناد اسم الصورة للخانة الجديدة
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
    // 👇 إضافة cardImageName و cardQuizTrack للقائمة للتصفير عند الإغلاق
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