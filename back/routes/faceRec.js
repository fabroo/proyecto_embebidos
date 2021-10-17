const fs = require('fs')
const AWS = require('aws-sdk');
const multer = require('multer')

var express = require('express');
const { authMiddleware } = require('../utils');

var router = express.Router();

//router.use(authMiddleware)

require("dotenv").config()
const {
    AWS_ID,
    AWS_SECRET,  
} = process.env
AWS.config.update({ region: 'us-east-1', credentials:{
    accessKeyId:AWS_ID,
    secretAccessKey:AWS_SECRET
} });
const rekognition = new AWS.Rekognition()

var uploading = multer({
    dest: 'data/uploads/',
})

router.delete('/deleteCollection', (req,res)=>{
    rekognition.deleteCollection({CollectionId:"monomon"}, (err, data)=>{
        if(err) return res.status(400).json({err:String(err)})
        else return res.status(200).json({data})
    })
}) 

router.post('/entryFace', uploading.single('img'), async (req, res) => {

    var tmp_path = req.file.path;
    var tmpPic = fs.readFileSync(tmp_path)
    fs.unlinkSync(tmp_path)
    
    var searchParams = {
        CollectionId: 'monomon',
        FaceMatchThreshold: 90,
        Image: {
            Bytes: tmpPic
        },
        MaxFaces: 5
    }
    var result = await searchByImage(searchParams, "45583265")  
    return res.json(result)
})
//
router.post('/addFaces', [authMiddleware, uploading.array('files[]', 3)], (req,res)=>{
    var files = req.files
    console.log("QUE", JSON.stringify(req.userData, null, 4))
    files.forEach((v,i,a)=>{
        var tmp_path = v.path
        var face = fs.readFileSync(tmp_path)
        var params = {
            CollectionId:'monomon',
            Image:{
                Bytes:face
            },
            ExternalImageId: req.userData.uid
        }
        rekognition.indexFaces(params, (err,data)=>{
            if(err) return res.status(400).json({err:true, errorMesage: String(err)})
        })
    })
})

router.get('/createCollection', (req,res)=>{
    rekognition.createCollection({
        CollectionId:"monomon"
    }, (err, data)=>{
        if(err) return res.status(400).json({err:String(err)})
        else return res.status(200).json({data})
    })
})

router.get('/listCollectionData', (req,res)=>{
    rekognition.listFaces({CollectionId:"monomon"}, (err,data)=>{
        return res.status(200).json(data)
    })
})

function searchByImage(params, id) {
    return new Promise(async (resolve, reject) => {
        console.log("arranca")
        rekognition.searchFacesByImage(params, async (err, data) => {
            if (err) {
                resolve(false)
            }
            else {
                resolve(data.FaceMatches.every(obj=>obj.Face.ExternalImageId == id))
            }
        })
    })
}

module.exports = router