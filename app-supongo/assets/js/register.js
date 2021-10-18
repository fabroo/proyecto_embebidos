const { initializeApp } = require('@firebase/app');
const auth = require('firebase/auth')
const db = require('firebase/firestore')

window.onload = function(){
    const firebaseConfig = {
        apiKey: "AIzaSyCbXifIKRwcow_QWtlxyjIti4f7IAMU9mE",
        authDomain: "monomon.firebaseapp.com",
        projectId: "monomon",
        storageBucket: "monomon.appspot.com",
        messagingSenderId: "936666137062",
        appId: "1:936666137062:web:d803283b04fbb131a59627"
    };
    
    initializeApp(firebaseConfig);
}

function register() {
    var authObject = auth.getAuth()
    var postData = document.getElementById("postData")
    var name = document.getElementById("name").value
    var email = document.getElementById("email").value
    // if(!emailValid(email)){
    //     postData.innerHTML = "Invalid email."
    //     return
    // }

    var password = document.getElementById("password").value
    var confirmPassword = document.getElementById("password").value
    if (password != confirmPassword) {
        postData.innerHTML = "Passwords don't match"
        return
    }
    auth.createUserWithEmailAndPassword(authObject, email, password)
    .then(user=>{
        auth.updateProfile(authObject.currentUser, {displayName: name})
        .then(()=>{
            postData.innerHTML = "User registered correctly. Welcome, " + authObject.currentUser.displayName
            localStorage.setItem("user", authObject.currentUser)
            setTimeout(()=>{
                window.location = '../index.html'
            }, 2000)
        })
        .catch((err)=> postData.innerHTML = String(err))
    })
}

//const emailValid = (email) => email.match(/^w+([.-]?w+)*@w+([.-]?w+)*(.w{2,3})+$/)