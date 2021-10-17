const { ipcRenderer } = require('electron')
const Pusher = require('pusher-js');

var pusher = new Pusher('b103ad2b1e20a1198455', {
    cluster: 'us2'
});
var channel = pusher.subscribe('monomon');

window.onload = function () {
    var form = document.getElementById("fileupload")
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
                "Authorization": "Bearer " + token
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

