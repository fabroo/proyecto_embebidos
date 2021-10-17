const express = require("express")
const faceRec = require('./routes/faceRec')
const cors = require("cors")
const app = express()

require("dotenv").config()
const admin = require('firebase-admin')
admin.initializeApp({
    credential: admin.credential.cert(JSON.parse(process.env.FIREBASE_DATA))
})

app.use(express.urlencoded({ extended: false }))
app.use(express.json())
app.use(cors());
app.use('/faceRec', faceRec)

app.listen(4022)