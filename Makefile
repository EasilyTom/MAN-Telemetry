default:
	echo "Please specify build file"

## Compilies all binaries and links them to mod_canusb
can:
	gcc -o mod_canusb canusb.c ecu_can_parser.c ecu_can_parser.h -lpthread
.PHONY:clean
clean:
	rm mod_canusb display