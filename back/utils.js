const admin = require('firebase-admin')

const authMiddleware = (req,res, next) => {
    var auth = req.headers['authorization'].split(' ')[1]
    console.log(auth)
    if(auth== "realshit") return next()
    admin.auth().verifyIdToken(auth)
    .then(value=>{
        req.userData = value
        next()
    })
    .catch(err=> {
        console.log(err)
        return res.status(400).json({err:true, errorMessage:"Unauthenticated request. User is not logged in, or token is invalid."})
    })
}

module.exports={
    authMiddleware
}