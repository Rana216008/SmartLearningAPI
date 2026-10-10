
//const translations = {
//    ar: {
//        title: "منصة التعلم الذكي",
//        navSubtitle: "Smart Learning Platform",
//        loginSubtitle: "Learning Robot",
//        userLabel: "اسم المستخدم",
//        passLabel: "كلمة المرور",
//        userPlaceholder: "أدخل اسم المستخدم",
//        passPlaceholder: "أدخل كلمة المرور",
//        button: "دخول",
//        pageTitle: "تسجيل الدخول - الروبوت التعليمي"
//    },
//    en: {
//        title: "Smart Learning Platform",
//        navSubtitle: "Smart Learning Platform",
//        loginSubtitle: "Learning Robot",
//        userLabel: "Username",
//        passLabel: "Password",
//        userPlaceholder: "Enter username",
//        passPlaceholder: "Enter password",
//        button: "Login",
//        pageTitle: "Login - Learning Robot"
//    }
//};

//function setLanguage(lang) {
//    localStorage.setItem('lang', lang);
//    const t = translations[lang] || translations['ar'];

//    if (document.getElementById('lang-label')) document.getElementById('lang-label').innerText = lang.toUpperCase();
//    if (document.getElementById('login-title')) document.getElementById('login-title').innerText = t.title;
//    if (document.getElementById('login-subtitle')) document.getElementById('login-subtitle').innerText = t.loginSubtitle;
//    if (document.getElementById('user-label')) document.getElementById('user-label').innerText = t.userLabel;
//    if (document.getElementById('pass-label')) document.getElementById('pass-label').innerText = t.passLabel;
//    if (document.getElementById('login-button')) document.getElementById('login-button').innerText = t.button;

//    if (document.getElementById('username')) document.getElementById('username').placeholder = t.userPlaceholder;
//    if (document.getElementById('password')) document.getElementById('password').placeholder = t.passPlaceholder;

//    document.documentElement.dir = (lang === 'ar') ? 'rtl' : 'ltr';
//    document.documentElement.lang = lang;
//    document.title = t.pageTitle;
//}

//document.addEventListener('DOMContentLoaded', function () {
//    // 1. التحكم بظهور/إخفاء كلمة المرور
//    const toggleEye = document.getElementById('toggleEye');
//    const passwordInput = document.getElementById('password');

//    if (toggleEye && passwordInput) {
//        toggleEye.addEventListener('click', function () {
//            const type = passwordInput.getAttribute('type') === 'password' ? 'text' : 'password';
//            passwordInput.setAttribute('type', type);
//            this.classList.toggle('fa-eye');
//            this.classList.toggle('fa-eye-slash');
//        });
//    }

//    // 2. التحكم بقائمة اللغات
//    const langBtn = document.getElementById('lang-btn');
//    const dropdown = document.getElementById('lang-dropdown');

//    if (langBtn && dropdown) {
//        langBtn.addEventListener('click', (e) => {
//            e.stopPropagation();
//            dropdown.classList.toggle('hide');
//        });

//        document.addEventListener('click', () => {
//            dropdown.classList.add('hide');
//        });

//        document.querySelectorAll('#lang-dropdown a').forEach(link => {
//            link.addEventListener('click', (e) => {
//                e.preventDefault();
//                const selectedLang = link.getAttribute('data-lang');
//                setLanguage(selectedLang);
//            });
//        });
//    }

  
document.addEventListener('DOMContentLoaded', function () {

    // --- 1. برمجة العين (إظهار/إخفاء) ---
    const toggleEye = document.getElementById('toggleEye');
    const passwordInput = document.getElementById('password');

    if (toggleEye && passwordInput) {
        toggleEye.addEventListener('click', function () {
            // تبديل النوع
            const type = passwordInput.getAttribute('type') === 'password' ? 'text' : 'password';
            passwordInput.setAttribute('type', type);

            // تبديل شكل الأيقونة (العين المفتوحة والمغلقة)
            this.classList.toggle('fa-eye');
            this.classList.toggle('fa-eye-slash');
        });
    }

    const translations = {
        ar: {
            title: "منصة التعلم الذكي",
            navSubtitle: "Smart Learning Platform",
            loginSubtitle: "Learning Robot",
            userLabel: "اسم المستخدم",
            passLabel: "كلمة المرور",
            userPlaceholder: "أدخل اسم المستخدم",
            passPlaceholder: "أدخل كلمة المرور",
            button: "دخول",
            pageTitle: "تسجيل الدخول - الروبوت التعليمي"
        },
        en: {
            title: "Smart Learning Platform",
            navSubtitle: "Smart Learning Platform",
            loginSubtitle: "Learning Robot",
            userLabel: "Username",
            passLabel: "Password",
            userPlaceholder: "Enter username",
            passPlaceholder: "Enter password",
            button: "Login",
            pageTitle: "Login - Learning Robot"
        }
    };

    function setLanguage(lang) {
        localStorage.setItem('lang', lang);
        const t = translations[lang];

        if (document.getElementById('lang-label')) document.getElementById('lang-label').innerText = lang.toUpperCase();
        if (document.getElementById('login-title')) document.getElementById('login-title').innerText = t.title;
        if (document.getElementById('login-subtitle')) document.getElementById('login-subtitle').innerText = t.loginSubtitle;
        if (document.getElementById('user-label')) document.getElementById('user-label').innerText = t.userLabel;
        if (document.getElementById('pass-label')) document.getElementById('pass-label').innerText = t.passLabel;
        if (document.getElementById('login-button')) document.getElementById('login-button').innerText = t.button;

        // تحديث الـ Placeholder
        if (document.getElementById('username')) document.getElementById('username').placeholder = t.userPlaceholder;
        if (document.getElementById('password')) document.getElementById('password').placeholder = t.passPlaceholder;

        // تحديث اتجاه الصفحة
        document.documentElement.dir = (lang === 'ar') ? 'rtl' : 'ltr';
        document.documentElement.lang = lang;
        document.title = t.pageTitle;
    }

    // --- 3. القائمة المنسدلة للغات ---
    const langBtn = document.getElementById('lang-btn');
    const dropdown = document.getElementById('lang-dropdown');

    if (langBtn) {
        langBtn.addEventListener('click', (e) => {
            e.stopPropagation();
            dropdown.classList.toggle('hide');
        });
    }

    document.addEventListener('click', () => {
        if (dropdown) dropdown.classList.add('hide');
    });

    document.querySelectorAll('#lang-dropdown a').forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const selectedLang = link.getAttribute('data-lang');
            setLanguage(selectedLang);
        });
    });

    // تحميل اللغة المحفوظة أو الافتراضية عند فتح الصفحة
    const savedLang = localStorage.getItem('lang') || 'ar';
    setLanguage(savedLang);
});