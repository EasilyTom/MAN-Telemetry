ecu_param_list = ["0"] * 11

canPipe = "/tmp/can_data_pipe"
print("Display test. Reading: " + canPipe)

def try_reading(): 
   try:
      with open(canPipe, "r") as pipe:
         global ecu_param_list
         data_buffer = pipe.read() #Okay this is reading properly. Fucking sexy
         ecu_param_list = data_buffer.split(',')
         
         print(ecu_param_list)
         
         if len(ecu_param_list) < 11:
             ecu_param_list.extend([None] * (11 - len(ecu_param_list)))
         
         # for x in ecu_param_list:
         #    print(x)
   except FileNotFoundError:
      print("Named pipe does not exist!")
   except OSError as e:
      print(f"Failed to read data from named pipe: {e}")
      
while True:
    try_reading()