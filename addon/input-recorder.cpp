#include <nan.h>
#include <ApplicationServices/ApplicationServices.h>

// The callback for when input events occur
CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  if (type != kCGEventKeyDown && type != kCGEventFlagsChanged && type != kCGEventKeyUp) { return event; }
  //printf("inside event\n");
  const Nan::AsyncProgressWorker::ExecutionProgress* p = (const Nan::AsyncProgressWorker::ExecutionProgress*)refcon;
  
  // Retrieve the incoming keycode.
  CGKeyCode keyCode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  //printf("%d\n", keyCode);
  p->Send(reinterpret_cast<const char*>(&keyCode), sizeof(int));

  return event;
}

class RecorderWorker : public Nan::AsyncProgressWorker {
public: 
  RecorderWorker(Nan::Callback *callback, Nan::Callback *prog) : 
    AsyncProgressWorkerBase(callback), prog(prog) {}

  ~RecorderWorker() {}

  void Execute(const Nan::AsyncProgressWorker::ExecutionProgress& progress) {
    printf("Doing some work\n");

    // Create an event tap to retrieve keypresses.
    //CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);
    CGEventMask eventMask = (CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged));
    //CGEventMask eventMask = CGEventMaskBit(kCGEventFlagsChanged);
    //void* data = 
    eventTap = CGEventTapCreate(
      kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, eventMask, CGEventCallback, (void*)&progress
    );
  
    // Exit the program if unable to create the event tap.
    if(!eventTap) {
        //fprintf(stderr, "ERROR: Unable to create event tap.\n");
        printf("ERROR: Unable to create event tap.\n");
    }else{
      CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
      loop = CFRunLoopGetCurrent();
      printf("%p\n", loop);
      CFRunLoopAddSource(loop, runLoopSource, kCFRunLoopDefaultMode);
      CGEventTapEnable(eventTap, true);

      //int i=69;
      //progress.Send(reinterpret_cast<const char*>(&i), sizeof(int));
      //progress.Signal();
  
      CFRunLoopRun();
    }
  }

  void StopLoop() {
    printf("STOPPING!");
    CFRunLoopStop(loop);
  }

  void HandleProgressCallback(const char *data, size_t count) {
    Nan::HandleScope scope;

    //printf("inside progress bit");

    CGKeyCode code = (CGKeyCode)(*data);

    v8::Local<v8::Value> argv[] = { Nan::New(code) };

    prog->Call(1, argv);
  }

  void HandleOKCallback () {
    Nan::HandleScope scope;

    printf("Inside ok, about to run callback\n");

    v8::Local<v8::Value> argv[] = {
      Nan::Null(),
      Nan::New(7)
    };

    callback->Call(2, argv);
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
  printf("About to stop the run loop\n");
  rw->StopLoop();
}

NAN_MODULE_INIT(Initialize) {
  NAN_EXPORT(target, startRecording);
  NAN_EXPORT(target, stopRecording);
}

NODE_MODULE(inputrecorder, Initialize);


/*
// The callback for when input events occur
CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  printf("inside event\n");
  if (type != kCGEventKeyDown && type != kCGEventFlagsChanged && type != kCGEventKeyUp) { return event; }

  // Retrieve the incoming keycode.
  CGKeyCode keyCode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  // Print the human readable key to the logfile.
  printf("%d\n", keyCode);
  //fflush(logfile);

  return event;
}

CFMachPortRef eventTap;

// NAN_METHOD is a Nan macro enabling convenient way of creating native node functions.
// It takes a method's name as a param. By C++ convention, I used the Capital cased name.
NAN_METHOD(Hello) {
      
  printf("Inside Hello");
  */
  /*if(CGEventTapIsEnabled(eventTap)){
    printf("the tap is enabled123123123123123!!##");
  }*/

  /*// Create an instance of V8's String type
  auto message = Nan::New("Hello from C++!").ToLocalChecked();
  // 'info' is a macro's "implicit" parameter - it's a bridge object between C++ and JavaScript runtimes
  // You would use info to both extract the parameters passed to a function as well as set the return value.
  info.GetReturnValue().Set(message);
  */
  /*
    // Create an event tap to retrieve keypresses.
    CGEventMask eventMask = (CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged));
    eventTap = CGEventTapCreate(
        kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, eventMask, CGEventCallback, NULL
    );
  
    // Exit the program if unable to create the event tap.
    if(!eventTap) {
        //fprintf(stderr, "ERROR: Unable to create event tap.\n");
        printf("ERROR: Unable to create event tap.\n");
        exit(1);
    }
  
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);
    if(CGEventTapIsEnabled(eventTap)){
      printf("the tap is enabled");
    }
    printf("%d\n", runLoopSource);
    printf("project started!");
}



// Module initialization logic
NAN_MODULE_INIT(Initialize) {
  // Export the `Hello` function (equivalent to `export function Hello (...)` in JS)
  NAN_EXPORT(target, Hello);
}

// Create the module called "inputrecorder" and initialize it with `Initialize` function (created with NAN_MODULE_INIT macro)
NODE_MODULE(inputrecorder, Initialize);
*/