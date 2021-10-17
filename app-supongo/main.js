const { app, BrowserWindow, ipcMain } = require('electron')
const { searchByImage } = require('./assets/js/awsHandler')
const { spawn } = require('child_process')
const Pusher = require('pusher-js');
const fs = require('fs')

let py;

var pusher = new Pusher('b103ad2b1e20a1198455', {
    cluster: 'us2'
});
var channel = pusher.subscribe('monomon');


function createWindow() {
    const win = new BrowserWindow({
        width: 800,
        height:600,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
        }
    })

    win.loadFile('assets/index.html')
}

app.on('window-all-closed', function () {
    if (process.platform !== 'darwin') app.quit()
})

app.whenReady().then(() => {
    createWindow()
    app.on('activate', function () {
        if (BrowserWindow.getAllWindows().length === 0) createWindow()
    })
})

ipcMain.on('runPython', async (e, req)=>{
    console.log("estamos")
    py = spawn('python', ['assets/python/main.py', 2])
})

ipcMain.on('stopPython', async (e, req)=>{
    py.kill()
})

ipcMain.on('faceDetected', async function (e, req) {
    var data = fs.readFileSync('assets/sendImage.png')
    console.log("holiwi")
    const {valid, uid} = await searchByImage(data)
    if(valid) {
        console.log(uid)
        channel.trigger
        e.reply('faceFound', {uid})
        
        //request al nodeMCU para empezar el rfid
    }
});





