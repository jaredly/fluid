#include "./fluid_shared.h"

// MARK - View operations

void fluid_NSView_appendChild(value view_v, value child_v) {
  CAMLparam2(view_v, child_v);
  log("Append child view\n");
  NSView* view = (NSView*) Unwrap(view_v);
  NSView* child = (NSView*) Unwrap(child_v);
  // NSLog(@"- unwrapped %@\n", view);

  NSView* last = view.subviews.lastObject;
  if (last != nil) {
    log("- add after\n");
    [view addSubview:child positioned:NSWindowBelow relativeTo:last];
  } else {
    log("- add last\n");
    [view addSubview:child];
  }

  CAMLreturn0;
}

void fluid_NSView_removeChild(value view_v, value child_v) {
  CAMLparam2(view_v, child_v);
  log("Remove child view\n");
  NSView* view = (NSView*) Unwrap(view_v);
  NSView* child = (NSView*) Unwrap(child_v);

  if (child.superview == view) {
    [child removeFromSuperview];
  }

  CAMLreturn0;
}

void fluid_NSView_replaceWith(value view_v, value replace_v) {
  CAMLparam2(view_v, replace_v);
  log("Replace view\n");
  NSView* view = (NSView*) Unwrap(view_v);
  NSView* replace = (NSView*) Unwrap(replace_v);

  if (view.superview != nil) {
    [view.superview replaceSubview:view with:replace];
  }

  CAMLreturn0;
}

void fluid_setImmediate(value callback) {
  CAMLparam1(callback);
  log("Set immediate\n");
  // dispatch_after(DISPATCH_TIME_NOW, dispatch_get_main_queue(), ^(void){
    caml_callback(callback, Val_unit);
  // });
  CAMLreturn0;
}

// MARK - location updates

void fluid_update_NSView_loc(value view_v, value dims_v) {
  CAMLparam2(view_v, dims_v);

  NSView* view = (NSView*) Unwrap(view_v);
  Unpack_record4_double(dims_v, left, top, width, height);

  [view setFrameOrigin:NSMakePoint(left, top)];
  [view setFrameSize:NSMakeSize(width, height)];

  CAMLreturn0;
}

void fluid_update_Text_loc(value view_v, value dims_v) {
  CAMLparam2(view_v, dims_v);

  NSTextField* text = (NSTextField*) Unwrap(view_v);
  Unpack_record4_double(dims_v, left, top, width, height);

  [text setFrameOrigin:NSMakePoint(left, top + 5.0)];
  [text setFrameSize:NSMakeSize(width, height)];

  CAMLreturn0;
}

void fluid_update_NSButton_loc(value view_v, value dims_v) {
  CAMLparam2(view_v, dims_v);

  FluidButton* view = (FluidButton*) Unwrap(view_v);
  Unpack_record4_double(dims_v, left, top, width, height);

  [view setFrameOrigin:NSMakePoint(left, top)];
  // [view setFrameSize:NSMakeSize(width, height)];

  CAMLreturn0;
}

// MARK - Image View

CAMLprim value fluid_create_NSImageView(value src_v, value dims_v) {
  CAMLparam2(src_v, dims_v);

  Unpack_record4_double(dims_v, left, top, width, height);

  NSRect frame = NSMakeRect(left, top, width, height);

  NSView* view = [[FlippedView alloc] initWithFrame:frame];
  NSImage* image = [[NSImage alloc] initWithContentsOfFile:NSString_val(src_v)];
  view.wantsLayer = true;
  if (image != nil) {
    [view.layer setContents:image];
  } else {
    view.layer.backgroundColor = CGColorCreateGenericRGB(1.0, 0, 0, 1.0);
  }

  CAMLlocal1(view_v);
  Wrap(view_v, view);
  CAMLreturn(view_v);
}

// MARK - Plain View

CAMLprim value fluid_create_NSView(value id, value pos_v, value size_v, value style_v) {
  CAMLparam4(id, pos_v, size_v, style_v);
  CAMLlocal1(view_v);

  Double_pair(pos_v, top, left);
  Double_pair(size_v, width, height);
  NSRect frame;
  frame = NSMakeRect(left, top, width, height);

  NSView* view = [[FlippedView alloc] initWithFrame:frame];

  value backgroundColor = Field(style_v, 0);
  if (Is_block(backgroundColor) && Tag_val(backgroundColor) == 0) {
    value color = Field(backgroundColor, 0);
    Unpack_record4_double(color, r, g, b, a);
    view.wantsLayer = true;
    view.layer.backgroundColor = CGColorCreateGenericRGB(r, g, b, a);
  }

  Wrap(view_v, view);
  CAMLreturn(view_v);
}

void fluid_update_NSView(value view_v, value id, value style_v) {
  CAMLparam3(view_v, id, style_v);

  NSView* view = (NSView*)Unwrap(view_v);
  printf("Update view\n");

  value backgroundColor = Field(style_v, 0);
  if (Is_block(backgroundColor) && Tag_val(backgroundColor) == 0) {
    value color = Field(backgroundColor, 0);
    Unpack_record4_double(color, r, g, b, a);
    view.wantsLayer = true;
    view.layer.backgroundColor = CGColorCreateGenericRGB(r, g, b, a);
  } else {
    view.wantsLayer = false;
  }

  CAMLreturn0;
}


// MARK - Text

CAMLprim value fluid_measureText(value text_v, value font_v, value fontSize_v, value maxWidth_v) {
  CAMLparam4(text_v, font_v, fontSize_v, maxWidth_v);
  CAMLlocal1(result);
  // TODO cache these values pls
  // log("Measure text\n");

  NSSize textSize;

  NSString *textContent = NSString_val(text_v);
  NSString *font = NSString_val(font_v);
  double fontSize = Double_val(fontSize_v);

  NSFont *nsFont = [NSFont fontWithName:font size:fontSize];
  if (nsFont == nil) {
    result = caml_alloc_tuple(2);
    printf("Invalid font %s\n", String_val (font_v));
    Store_field (result, 0, caml_copy_double(0.f));
    Store_field (result, 1, caml_copy_double(0.f));

    CAMLreturn(result);
  }

  if (Is_block(maxWidth_v)) {
    float maxWidth = Double_val(Field(maxWidth_v, 0));

    NSTextStorage *textStorage = [[[NSTextStorage alloc]
          initWithString:textContent] autorelease];
    NSTextContainer *textContainer = [[[NSTextContainer alloc]
          initWithContainerSize: NSMakeSize(maxWidth, FLT_MAX)] autorelease];
    NSLayoutManager *layoutManager = [[[NSLayoutManager alloc] init]
          autorelease];
    [layoutManager addTextContainer:textContainer];
    [textStorage addLayoutManager:layoutManager];
    [textStorage addAttribute:NSFontAttributeName value:nsFont
          range:NSMakeRange(0, [textStorage length])];
    [textContainer setLineFragmentPadding:2.0];
    (void) [layoutManager glyphRangeForTextContainer:textContainer];
    textSize = [layoutManager usedRectForTextContainer:textContainer].size;
  } else {
    NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:nsFont, NSFontAttributeName, nil];
    NSAttributedString *text = [[NSAttributedString alloc] initWithString:textContent attributes: attributes];
    textSize = [text size];
  }

  Create_double_pair(result, textSize.width + 5.0, textSize.height + 5.0);

  CAMLreturn(result);
}

CAMLprim value fluid_create_NSTextView(value contents_v, value dims_v, value font_v) {
  CAMLparam3(contents_v, dims_v, font_v);
  CAMLlocal1(text_v);
  log("Create text view\n");

  NSString *contents = NSString_val(contents_v);
  NSTextField* text;

  NSString *fontName = NSString_val(Field(font_v, 0));
  double fontSize = Double_val(Field(font_v, 1));

  NSFont *nsFont = [NSFont fontWithName:fontName size:fontSize];
  if (nsFont != nil) {
    NSMutableAttributedString *attrstr = [[NSMutableAttributedString alloc] initWithString:contents];
    NSDictionary *attributes = @{ NSFontAttributeName : nsFont };
    [attrstr setAttributes:attributes range:NSMakeRange(0, contents.length)];
    text = [NSTextField labelWithAttributedString:attrstr];
  } else {
    text = [NSTextField labelWithString:contents];
  }

  Unpack_record4_double(dims_v, left, top, width, height);

  [text setFrameOrigin:NSMakePoint(left, top + 5.0)];
  [text setFrameSize:NSMakeSize(width, height)];

  // text.wantsLayer = true;
  // text.layer.backgroundColor = CGColorCreateGenericRGB(0, 1, 0, 1);
  Wrap(text_v, text);

  CAMLreturn(text_v);
}

void fluid_set_NSTextView_textContent(value text_v, value contents_v, value dims, value font_v) {
  CAMLparam4(text_v, contents_v, dims, font_v);
  log("Update text view\n");

  NSTextField* text = (NSTextField*)Unwrap(text_v);
  NSString *contents = NSString_val(contents_v);

  NSString *fontName = NSString_val(Field(font_v, 0));
  double fontSize = Double_val(Field(font_v, 1));

  NSFont *nsFont = [NSFont fontWithName:fontName size:fontSize];
  if (nsFont != nil) {
    NSMutableAttributedString *attrstr = [[NSMutableAttributedString alloc] initWithString:contents];
    NSDictionary *attributes = @{ NSFontAttributeName : nsFont };
    [attrstr setAttributes:attributes range:NSMakeRange(0, contents.length)];
    text.attributedStringValue = attrstr;
  } else {
    text.stringValue = contents;
  }

  Unpack_record4_double(dims, left, top, width, height);

  [text setFrameOrigin:NSMakePoint(left, top + 5.0)];
  [text setFrameSize:NSMakeSize(width, height)];

  CAMLreturn0;
}

// MARK - Button


void fluid_update_NSButton(value button_v, value title_v) {
  CAMLparam2(button_v, title_v);
  log("Update button\n");

  FluidButton* button = (FluidButton*)Unwrap(button_v);
  NSString *title = NSString_val(title_v);

  button.title = title;

  // float top = Double_val(Field(pos_v, 0));
  // float left = Double_val(Field(pos_v, 1));

  // [button setFrameOrigin:NSMakePoint(left, top)];
  // [button setFrameSize:NSMakeSize(width, height)];

  CAMLreturn0;
}

CAMLprim value fluid_create_NSButton(value title_v, value id, value pos_v, value size_v) {
  CAMLparam4(title_v, id, pos_v, size_v);
  CAMLlocal1(button_v);
  log("Create button\n");

  NSString *title = NSString_val(title_v);

  FluidButton* button = [FluidButton createWithTitle:title id:Int_val(id)];

  Double_pair(pos_v, top, left);
  // Double_pair(size_v, width, height);

  [button setFrameOrigin:NSMakePoint(left, top)];

  Wrap(button_v, button);
  CAMLreturn(button_v);
}

// MARK - Null node

CAMLprim value fluid_create_NullNode() {
  CAMLparam0();
  CAMLlocal1(view_v);
  NSView* view = [[FlippedView alloc] initWithFrame:CGRectZero];
  Wrap(view_v, view);
  CAMLreturn(view_v);
}
