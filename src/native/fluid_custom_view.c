#include "./fluid_shared.h"


@interface CustomView: NSView
  @property (nonatomic) int onDraw;
  @property (nonatomic) int onMouseDown;
  @property (nonatomic) int onMouseUp;
  @property (nonatomic) int onMouseMove;
  @property (nonatomic) int onMouseDragged;
  @property (nonatomic) int onRightMouseDown;
@end

@implementation CustomView {
  NSTrackingArea* trackingArea;
}

-(void)updateTrackingAreas { 
    [super updateTrackingAreas];
    if (trackingArea != nil) {
        [self removeTrackingArea:trackingArea];
        [trackingArea release];
    }

    int opts = (NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveInKeyWindow);
    trackingArea = [ [NSTrackingArea alloc] initWithRect:[self bounds]
                                                 options:opts
                                                   owner:self
                                                userInfo:nil];
    [self addTrackingArea:trackingArea];
}

- (void)mouseMoved:(NSEvent *)event {
  if (self.onMouseMove != -1) {
    NSPoint local = [self convertPoint:event.locationInWindow fromView:nil];
    callPos(self.onMouseMove, local.x, local.y);
  }
}

- (void)mouseDragged:(NSEvent *)event {
  if (self.onMouseDragged != -1) {
    NSPoint local = [self convertPoint:event.locationInWindow fromView:nil];
    callPos(self.onMouseDragged, local.x, local.y);
  }
}

- (void)mouseDown:(NSEvent *)event {
  if (self.onMouseDown != -1) {
    NSPoint local = [self convertPoint:event.locationInWindow fromView:nil];
    callPos(self.onMouseDown, local.x, local.y);
  }
}

- (void)rightMouseDown:(NSEvent *)event {
  if (self.onRightMouseDown != -1) {
    NSPoint local = [self convertPoint:event.locationInWindow fromView:nil];
    callPos(self.onRightMouseDown, local.x, local.y);
  }
}

- (void)mouseUp:(NSEvent *)event {
  if (self.onMouseUp != -1) {
    NSPoint local = [self convertPoint:event.locationInWindow fromView:nil];
    callPos(self.onMouseUp, local.x, local.y);
  }
}

- (BOOL)isFlipped {
  return YES;
}

- (void)drawRect:(NSRect)dirtyRect {
  callRect(
    self.onDraw,
    dirtyRect.origin.x,
    dirtyRect.origin.y,
    dirtyRect.size.width,
    dirtyRect.size.height
  );
}

@end


CAMLprim value fluid_create_CustomView(value dims_v, value draw_v, value handlers) {
  CAMLparam2(dims_v, draw_v);
  CAMLlocal1(view_v);
  // caml_register_global_root(&draw_v);
  log("Create custom view\n");

  Unpack_record4_double(dims_v, left, top, width, height);

  NSRect frame = NSMakeRect(left, top, width, height);
  CustomView* view = [[CustomView alloc] initWithFrame:frame];
  view.onDraw = Int_val(draw_v);
  view.onMouseDown = Check_optional(Field(handlers, 0)) ? Int_val(Unpack_optional(Field(handlers, 0))) : -1;
  view.onMouseUp = Check_optional(Field(handlers, 1)) ? Int_val(Unpack_optional(Field(handlers, 1))) : -1;
  view.onMouseMove = Check_optional(Field(handlers, 2)) ? Int_val(Unpack_optional(Field(handlers, 2))) : -1;
  view.onMouseDragged = Check_optional(Field(handlers, 3)) ? Int_val(Unpack_optional(Field(handlers, 3))) : -1;
  view.onRightMouseDown = Check_optional(Field(handlers, 4)) ? Int_val(Unpack_optional(Field(handlers, 4))) : -1;
  view.wantsLayer = true;

  Wrap(view_v, view);
  CAMLreturn(view_v);
}

void fluid_update_CustomView(value view_v, value draw_v, value handlers, value invalidated) {
  CAMLparam2(view_v, draw_v);
  CAMLlocal2(contents, c2);
  log("Update custom view\n");
  // caml_register_global_root(&draw_v);

  CustomView* view = (CustomView*)Unwrap(view_v);
  view.onDraw = Int_val(draw_v);
  view.onMouseDown = Check_optional(Field(handlers, 0)) ? Int_val(Unpack_optional(Field(handlers, 0))) : -1;
  view.onMouseUp = Check_optional(Field(handlers, 1)) ? Int_val(Unpack_optional(Field(handlers, 1))) : -1;
  view.onMouseMove = Check_optional(Field(handlers, 2)) ? Int_val(Unpack_optional(Field(handlers, 2))) : -1;
  view.onMouseDragged = Check_optional(Field(handlers, 3)) ? Int_val(Unpack_optional(Field(handlers, 3))) : -1;
  view.onRightMouseDown = Check_optional(Field(handlers, 4)) ? Int_val(Unpack_optional(Field(handlers, 4))) : -1;

  if (Check_optional(invalidated)) {
    contents = Unpack_optional(invalidated);
    if (Check_optional(contents)) {
      c2 = Unpack_optional(contents);
      int dims = Wosize_val(c2);
      for (int i=0; i<dims; i++) {
        // printf("Drawing %d\n", i);
        Unpack_record4_double(Field(c2, i), left, top, width, height);
        [view setNeedsDisplayInRect:CGRectMake(left, top, width, height)];
      }
    } else {
      [view setNeedsDisplayInRect:CGRectMake(0, 0, view.frame.size.width, view.frame.size.height)];
    }
  }

  CAMLreturn0;
}




void fluid_Draw_fillRect(value rect, value color) {
  CAMLparam2(rect, color);

  Unpack_record4_double(rect, left, top, width, height);
  Unpack_record4_double(color, r, g, b, a);
  [[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a] set];
  NSRectFill(NSMakeRect(left, top, width, height));

  CAMLreturn0;
}

void fluid_Draw_rect(value rect, value color) {
  CAMLparam2(rect, color);

  Unpack_record4_double(rect, left, top, width, height);
  Unpack_record4_double(color, r, g, b, a);
  [[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a] set];
  NSFrameRect(NSMakeRect(left, top, width, height));

  CAMLreturn0;
}

void fluid_Draw_text(value text, value pos, value font, value fontSize_v) {
  CAMLparam4(text, pos, font, fontSize_v);
  NSString* fontName = NSString_val(font);
  double fontSize = Double_val(fontSize_v);
  NSFont *nsFont = [NSFont fontWithName:NSString_val(font) size:fontSize];
  if (nsFont == nil) {
    nsFont = [NSFont systemFontOfSize:fontSize];
  }
  [NSString_val(text)
    drawAtPoint:CGPointMake(
      Double_field(pos, 0),
      Double_field(pos, 1)
    )
    withAttributes:@{NSFontAttributeName: nsFont}];

  CAMLreturn0;
}
