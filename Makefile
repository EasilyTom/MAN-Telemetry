default:
	gcc -o mod_canusb canusb.c

.PHONY:clean
clean:
	rm mod_canusb