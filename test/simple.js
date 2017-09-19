
var recorder = require('../index')

function cbDone(a, b) {
  console.log('-- Recorder has been turned off --')
}

function cbDetect(event, key) {
  console.log('-- event (' + event + ') has been detected -- ')
  // stop when q is pressed
  if(key === 12){
    recorder.stopRecording()
  }
}

recorder.startRecording(cbDone, cbDetect)
console.log('-- recorder has started --')