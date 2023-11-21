default:
	echo "Please specify build file"
can:
	gcc -o mod_canusb canusb.c

display: 
	rm display\
	gcc -o display display.c -lGL -lGLU -lglut -lm
.PHONY:clean
clean:
	rm mod_canusb display