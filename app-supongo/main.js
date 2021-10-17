const { app, BrowserWindow, ipcMain } = require('electron')
const { searchByImage } = require('./assets/js/awsHandler')
const { spawn } = require('child_process')
const fs = require('fs')

let py;

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
    const {valid, uid} = await searchByImage(data)
    if(valid) {
        e.reply('faceFound', {uid})
        //chequear si el usuario tiene rfids registrados, si tienee, mandarle al rfid que espere un rfid especifico
        //si no tiene, revertimos al estado 0 y le decimos al usuario que registre un rfid
    }
});





