const AWS = require('aws-sdk');
require('dotenv').config()
const {
    AWS_ID,
    AWS_SECRET,  
} = process.env
AWS.config.update({ region: 'us-east-1', credentials:{
    accessKeyId:AWS_ID,
    secretAccessKey:AWS_SECRET
} });
const rekognition = new AWS.Rekognition()

function searchByImage(data) {
    return new Promise(async (resolve, reject) => {
        var params = {
            CollectionId: 'monomon',
            FaceMatchThreshold: 90,
            Image: {
                Bytes: data
            },
            MaxFaces: 1
        }
        rekognition.searchFacesByImage(params, async (err, data) => {
            if (err) {
                console.log(err)
                resolve(false)
            }
            else {
                var set = new Set(data.FaceMatches.map(a=>a.Face.ExternalImageId))
                resolve({valid: set.size == 1, uid:data.FaceMatches[0].Face.ExternalImageId})
            }
        })
    })
}

module.exports = {
    searchByImage
}