var recorder = require('./build/Release/input-recorder.node')

// consts for event types
exports.events = {
  LEFT_MOUSE_DOWN: 1,
  RIGHT_MOUSE_DOWN: 3,
  KEY_DOWN: 10,
  MODIFIER_CHANGE: 12
}

exports.startRecording = function(done, detect) {
  recorder.startRecording(done, detect)
}

exports.stopRecording = function() {
  recorder.stopRecording()
}
