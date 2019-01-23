#include "./fluid_shared.h"
#import <Carbon/Carbon.h>

OSStatus OnHotKeyEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData) {
  EventHotKeyID hkCom;

  GetEventParameter(theEvent, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(hkCom), NULL, &hkCom);
  // AppController *controller = (AppController *)userData;  

  static value * closure_f = NULL;
  if (closure_f == NULL) {
      closure_f = caml_named_value("fluid_hotkeys_triggered");
  }
  caml_callback(*closure_f, Val_int(hkCom.id));

  return noErr;
}

void fluid_Hotkeys_init() {
  EventTypeSpec eventType;
  eventType.eventClass=kEventClassKeyboard;
  eventType.eventKind=kEventHotKeyPressed;    

  InstallApplicationEventHandler(&OnHotKeyEvent, 1, &eventType, NULL, NULL);
}

// Find keycodes in /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Headers/Events.h

void fluid_Hotkeys_register(value id_v, value key_v) {   
  CAMLparam2(id_v, key_v);

  EventHotKeyRef gMyHotKeyRef;
  EventHotKeyID gMyHotKeyID;

  gMyHotKeyID.signature='hotk';
  gMyHotKeyID.id=Int_val(id_v);
  RegisterEventHotKey(Int_val(key_v), cmdKey+optionKey, gMyHotKeyID, GetApplicationEventTarget(), 0, &gMyHotKeyRef);

  // RegisterEventHotKey(0x31, cmdKey+optionKey, gMyHotKeyID, GetApplicationEventTarget(), 0, &gMyHotKeyRef);  
  // gMyHotKeyID.signature='htk2';
  // gMyHotKeyID.id=2;   
  // RegisterEventHotKey(21, cmdKey+optionKey, gMyHotKeyID, GetApplicationEventTarget(), 0, &gMyHotKeyRef);  

  // gMyHotKeyID.signature='htk3';
  // gMyHotKeyID.id=3;   
  // RegisterEventHotKey(23, cmdKey+optionKey, gMyHotKeyID, GetApplicationEventTarget(), 0, &gMyHotKeyRef);  
  CAMLreturn0;
}

