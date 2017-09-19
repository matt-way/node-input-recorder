#include <nan.h>
#include <ApplicationServices/ApplicationServices.h>

// object representing key/mouse input details
struct input_t {
  CGEventType event;
  CGKeyCode key;
};

typedef Nan::AsyncProgressWorkerBase<input_t> AsyncInputWorker;

class RecorderWorker : public AsyncInputWorker {
public: 
  RecorderWorker(Nan::Callback *callback, Nan::Callback *prog) : 
    AsyncInputWorker(callback), prog(prog) {}

  ~RecorderWorker() {}

  // OSX event callback for peripheral interactions
  static CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {

    CGKeyCode key = 0;
    if(type == kCGEventFlagsChanged || type == kCGEventKeyDown || type == kCGEventKeyUp) {
      // Retrieve the incoming keycode.
      key = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);  
    }

    input_t ii;
    ii.event = type;
    ii.key = key;

    const AsyncInputWorker::ExecutionProgress* p = (const AsyncInputWorker::ExecutionProgress*)refcon;
    p->Send(&ii, 1);
    
    return event;
  }

  void Execute(const AsyncInputWorker::ExecutionProgress& progress) {
    // Create an event tap to retrieve all necessary events
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged) | 
                            CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventRightMouseDown);
    
    eventTap = CGEventTapCreate(
      kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, eventMask, CGEventCallback, (void*)&progress
    );
    
    // Exit the program if unable to create the event tap.
    if(!eventTap) {
        printf("ERROR: Unable to create event tap.\n");
    }else{
      CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
      loop = CFRunLoopGetCurrent();
      CFRunLoopAddSource(loop, runLoopSource, kCFRunLoopDefaultMode);
      CGEventTapEnable(eventTap, true);

      // block the thread so that all incoming events are handled
      CFRunLoopRun();
    }
  }

  void StopLoop() {
    CFRunLoopStop(loop);
  }

  void HandleProgressCallback(const input_t *data, size_t count) {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[] = {
      Nan::New(data->event),
      Nan::New(data->key)
    };

    prog->Call(2, argv);
  }

  void HandleOKCallback () {
    Nan::HandleScope scope;
    callback->Call(0, 0);
  }
  
private:
  Nan::Callback *prog;
  CFMachPortRef eventTap;
  CFRunLoopRef loop;
};

RecorderWorker *rw;

NAN_METHOD(startRecording) {
  Nan::Callback *cbCompleted = new Nan::Callback(info[0].As<v8::Function>());
  Nan::Callback *cbDetect = new Nan::Callback(info[1].As<v8::Function>());
  rw = new RecorderWorker(cbCompleted, cbDetect);
  AsyncQueueWorker(rw);
}

NAN_METHOD(stopRecording) {
  rw->StopLoop();
}

NAN_MODULE_INIT(Initialize) {
  NAN_EXPORT(target, startRecording);
  NAN_EXPORT(target, stopRecording);
}

NODE_MODULE(inputrecorder, Initialize);
