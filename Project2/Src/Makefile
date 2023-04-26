# Select your CPU type
CPU = cortex-a72
# CPU = cortex-a53

CC = gcc
UNROLL_CFLAGS = # -fno-unroll-loops
# VECTORIZE_CFLAGS =  -fno-tree-vectorize
VECTORIZE_CFLAGS =  -ftree-vectorize -mvectorize-with-neon-quad

MATH_CFLAGS =  -ffast-math -ffinite-math-only -fno-math-errno \
 -fsingle-precision-constant

WARNINGS = -Wno-unused-variable # // -Wall

CFLAGS = -mfpu=crypto-neon-fp-armv8 -mfloat-abi=hard \
-mcpu=$(CPU) -O3 -Ofast -ggdb -g $(UNROLL_CFLAGS) $(VECTORIZE_CFLAGS) \
$(MATH_CFLAGS) $(WARNINGS)

OBJECTS = yuv.o image_proc.o ansi_escapes.o main.o is_options.o

imstab: $(OBJECTS)
	$(CC) $(OBJECTS) -g -static -lrt -lm -o $@
#	cp -f imstab ../imstab

.c.o:
	$(CC) $(CFLAGS) -c $<
.c.s:
	$(CC) $(CFLAGS) -S $<

clean:
	rm -f imstab $(OBJECTS) 

