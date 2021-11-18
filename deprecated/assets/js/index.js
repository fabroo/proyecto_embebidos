const { ipcRenderer } = require('electron')
const Pusher = require('pusher-js');
const firebase = require('firebase/app')
const auth = require('firebase/auth')

var pusher = new Pusher('b103ad2b1e20a1198455', {
    cluster: 'us2'
});
var channel = pusher.subscribe('monomon');

window.onload = function () {
    const firebaseConfig = {
        apiKey: "AIzaSyCbXifIKRwcow_QWtlxyjIti4f7IAMU9mE",
        authDomain: "monomon.firebaseapp.com",
        projectId: "monomon",
        storageBucket: "monomon.appspot.com",
        messagingSenderId: "936666137062",
        appId: "1:936666137062:web:d803283b04fbb131a59627"
    };
    
    firebase.initializeApp(firebaseConfig);
    const a = auth.getAuth()


    var form = document.getElementById("fileupload")
    var logged  = document.getElementById('logged')
    form.addEventListener('submit', async function () {
        alert("woa")
        var files = document.getElementById("files").files
        var token = document.getElementById("token").value
        var fd = new FormData()
        for (const file of files) {
            fd.append('files[]', file)
        }
        const response = await fetch('http://localhost:4022/faceRec/addFaces', {
            method: "POST",
            headers: {
                "Authorization": "Bearer " + a.currentUser.getIdToken()
            },
            mode: "cors",
            body: fd
        })
        console.log(response.body)
    })
    ipcRenderer.on('faceFound', (e,req)=>{
        alert("aja?")
        var cara = document.getElementById("cara")
        cara.innerHTML = "HAY CARA: " + req.uid
    })


    
    var loginForm = document.getElementById("submit")
    loginForm.addEventListener("click", () => {
        var email = document.getElementById("email").value
        var password = document.getElementById("password").value
    
        auth.signInWithEmailAndPassword(a, email, password)
            .catch(err => logged.innerHTML = String(err))
    })
    auth.onAuthStateChanged(a, (user)=>{
        
        if(user){
            logged.innerHTML = "Loggeado, bienvenido: " + user.displayName
        }
    })
}



channel.bind('faceDetectedPython', async function (data) {
    ipcRenderer.send('faceDetected')
    var cara = document.getElementById("cara")
    cara.innerHTML = "Cara detectada, procesando..."
});

function startProcess() {
    ipcRenderer.send('runPython')
    alert("python started")
}

