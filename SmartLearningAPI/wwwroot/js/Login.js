function setLanguage(lang) {
    localStorage.setItem('lang', lang);
    document.getElementById('lang-label').innerText = lang.toUpperCase();

    if (lang === 'ar') {
        document.getElementById('login-subtitle').innerText = 'تسجيل الدخول';
        document.getElementById('password-label').innerText = 'كلمة المرور';
        document.getElementById('login-button').innerText = 'دخول';
        document.documentElement.dir = 'rtl';
        document.documentElement.lang = 'ar';
    } else {
        document.getElementById('login-subtitle').innerText = 'Login';
        document.getElementById('password-label').innerText = 'Password';
        document.getElementById('login-button').innerText = 'Login';
        document.documentElement.dir = 'ltr';
        document.documentElement.lang = 'en';
    }
}

const langBtn = document.getElementById('lang-btn');
const dropdown = document.getElementById('lang-dropdown');

langBtn.addEventListener('click', () => {
    dropdown.classList.toggle('hide');
});

document.addEventListener('click', (e) => {
    if (!e.target.closest('.language-toggle')) {
        dropdown.classList.add('hide');
    }
});

dropdown.querySelectorAll('a').forEach(link => {
    link.addEventListener('click', (e) => {
        e.preventDefault();
        const selectedLang = link.getAttribute('data-lang');
        setLanguage(selectedLang);
        dropdown.classList.add('hide');
    });
});

document.addEventListener('DOMContentLoaded', () => {
    const lang = localStorage.getItem('lang') || 'en';
    setLanguage(lang);
});