CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=c99 -pg

emulate: emulateMain.o executeFunction.o fetchedStruct.o binaryFileReader.o binMethods.o decodedStruct.o

binaryFileReader.o: binaryFileReader.c binaryFileReader.h machineStruct.h

binMethods.o: emulateEnums.h binMethods.h machineStruct.h shiftStruct.h

emulateMain.o: emulateEnums.h machineStruct.h binMethods.h executeFunction.h decodedStruct.h fetchedStruct.h binaryFileReader.h

executeFunction.o: emulateEnums.h machineStruct.h binMethods.h executeFunction.h decodedStruct.h fetchedStruct.h

fechedStruct.o: emulateEnums.h machineStruct.h binMethods.h fetchedStruct.h

decodedStruct.o: machineStruct.h emulateEnums.h decodedStruct.h binMethods.h shiftStruct.h

clean:
	$(RM) emulate *.o *~
	
.PHONY: clean

