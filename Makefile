
effecter: effecter.o ampMod.o overDrive.o reverb.o ringMod.o vibrato.o fuzzBox.o
	g++ -o effecter effecter.o ampMod.o overDrive.o reverb.o ringMod.o vibrato.o fuzzBox.o -lm -lportaudio -lmraa -lupm-i2clcd

effecter.o: effecter.cc config.h
	g++ -c effecter.cc

ampMod.o: effects/ampMod.c
	g++ -c effects/ampMod.c

overDrive.o: effects/overDrive.c
	g++ -c effects/overDrive.c 

reverb.o: effects/reverb.c
	g++ -c effects/reverb.c

ringMod.o: effects/ringMod.c
	g++ -c effects/ringMod.c

vibrato.o: effects/vibrato.c
	g++ -c effects/vibrato.c

fuzzBox.o: effects/fuzzBox.c
	g++ -c effects/fuzzBox.c


.PHONY: clean
clean:
	rm -f *.o effecter

