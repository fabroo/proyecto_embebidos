var form = document.getElementById("fileupload")
form.addEventListener('submit', async function(){
    alert("woa")
    var files = document.getElementById("files").files
    var token = document.getElementById("token").value
    var fd = new FormData()
    for (const file of files) {
        fd.append('files[]', file)
    }
    const response = await fetch('http://localhost:4022/faceRec/addFaces', {
        method:"POST",
        headers:{
            "Authorization": "Bearer " + token
        },
        mode:"cors",
        body:fd
    })
    console.log(response.body)
})