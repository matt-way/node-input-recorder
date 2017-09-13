var recorder = require('./build/Debug/input-recorder.node')

function cbDone(a, b) {
  console.log('-- cbDone called!')
  console.log(a, b)
}

function cbDetect(val) {
  //console.log('-- cbDetect called')
  //console.log(val)
  console.log(String.fromCharCode(val))
  if(val === 12){
    recorder.stopRecording()
  }
}

recorder.startRecording(cbDone, cbDetect)
