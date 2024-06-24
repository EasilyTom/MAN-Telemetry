import os
import time

pipe_path = "/tmp/can_data_pipe"

def show_data(datatoparse):

   print("RPM: ", datatoparse[0])
   print("Throttle: ", datatoparse[3])
   print("Battery: ", datatoparse[1])
   print("Something else: ", datatoparse[2])




def try_reading(): # This can be copied and pasted
   try:
      with open(pipe_path, "r") as pipe:
         data_buffer = pipe.read() #Okay this is reading properly. Fucking sexy
         split_data = data_buffer.split(',')
         show_data(datatoparse=split_data)

   except FileNotFoundError:
      print("Named pipe does not exist!")
   except OSError as e:
      print(f"Failed to read data from named pipe: {e}")

while(1):
   try_reading()
   print("----------------------------------------------------------")
   time.sleep(1)


   