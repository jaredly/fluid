src = $(wildcard *.m)
obj = $(src:.m=.o)

all: libfluid.so libfluid.a

libfluid.so: $(src)
	gcc -shared -o libfluid.so -fPIC $(src) \
		-I $(OCAMLLIB) -L$(OCAMLLIB) -lasmrun_shared \
		-framework Cocoa -framework CoreImage -framework Carbon

libfluid.a: $(obj)
	ar rcs libfluid.a $(obj)

%.o: %.m
	gcc -c -g $< -o $@ -I $(OCAMLLIB)

