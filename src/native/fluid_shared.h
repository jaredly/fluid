// #include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include <math.h>

#define CAML_NAME_SPACE

#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/callback.h>

// #include <string.h> // for memcpy
#include <caml/alloc.h>

#define NSString_val(v) [NSString stringWithUTF8String:String_val(v)]

#define Wrap(res, v) \
  res = caml_alloc(1, Abstract_tag); \
  Store_field(res, 0, (long)v)
#define Unwrap(value) \
  (void*)Field(value, 0)

// #define Wrap(res, v) res = (value) v
// #define Unwrap(value) value


#define Double_pair(c, a, b) \
  float a = Double_val(Field(c, 0)); \
  float b = Double_val(Field(c, 1))

#define Create_double_pair(c, a, b) \
  c = caml_alloc_tuple(2); \
  Store_field(c, 0, caml_copy_double(a)); \
  Store_field(c, 1, caml_copy_double(b))

#define DEBUG
// #define log(fmt) fprintf(stdout, fmt)
// #define logf(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

#define log(fmt) (void)0
#define logf(fmt, ...) (void)0

#define Unpack_record2_double(source, a, b) \
  float a = Double_field(source, 0); \
  float b = Double_field(source, 1)

#define Unpack_record4_double(source, a, b, c, d) \
  float a = Double_field(source, 0); \
  float b = Double_field(source, 1); \
  float c = Double_field(source, 2); \
  float d = Double_field(source, 3)

// #define Unpack_record4_double(source, a, b, c, d) float a = Double_field(source, 0); float b = Double_field(source, 1); float c = Double_field(source, 2); float d = Double_field(source, 3)
#define Check_optional(source) Is_block(source) && Tag_val(source) == 0
#define Unpack_optional(source) Field(source, 0)

@interface FlippedView : NSView
- (BOOL)isFlipped;
@end

@interface FluidButton : NSButton
+ (FluidButton*)createWithTitle:(NSString*)title id:(int)id;
- (void)setId:(int)id;
@end