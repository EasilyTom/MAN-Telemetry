# Author : Malhaar Karandikar 
# Org : Manchester Stingers
# Brief: It reads from a named pipe created by a c file and uses that data to dislay values to
#        the dahsboard


import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *
import os
import time 
import random 

canPipe = "/tmp/can_data_pipe"
print("Display test. Reading: " + canPipe)
# os.system("./canusb -d dev/ttyUSB0 -s 1000000") # This can also run the cansub. Bash script not necessary

ecu_param_list = ["0", "0", "0", "0"] # Initing a global array 

def try_reading(): 
   try:
      with open(canPipe, "r") as pipe:
         global ecu_param_list
         data_buffer = pipe.read() #Okay this is reading properly. Fucking sexy
         ecu_param_list = data_buffer.split(',')
         # for x in ecu_param_list:
         #    print(x)
   except FileNotFoundError:
      print("Named pipe does not exist!")
   except OSError as e:
      print(f"Failed to read data from named pipe: {e}")

def update_func():
   try_reading()
   # for x in ecu_param_list:
   #          print(x)

   tempRPM = int(ecu_param_list[0]) # Needed to do further calculations

   RPM.config(text= ecu_param_list[0])
   
   Throttle.config(text = ecu_param_list[1])
   
   Battery.config(text = ecu_param_list[3])  

   RPMBar.start()
   RPMBar.stop()
   
   #RPM bar colour and width calculations
   # if tempRPM <= 6000:
   #    RPMBar.config(bg = "green")
   # elif (tempRPM > 6000) and (tempRPM <= 7500):
   #    RPMBar.config(bg = "yellow")
   # else:
   #    RPMBar.config(bg = "red")
   
   # RPMBar.config(width = int(tempRPM/85))  
   # window.after(50, update_func)



window = tk.Tk(className="Telemetry display")



# Set window size
window.geometry("1800x1080")

# Set window color
window.configure(bg='black')

bg_image = tk.PhotoImage(file = "dash_display/Stingers_Dash.png") 

label1 = tk.Label( window, image = bg_image) 
label1.place(x = 0, y = 0) 

# Gear = tk.Label(window, padx = 45, text = "0", bg = '#A600C8', fg = "white", font = ("Ubuntu", 52))
# Gear.place(x = 317, y = 10)

Battery = tk.Label(window  ,padx = 10, text = "0.0", bg = "#404040", fg = "white", font= ("Ubuntu", 30))

RPM = tk.Label(window, padx = 10, text = "1000", bg = '#404040', fg = "white", font = ("Ubuntu", 30))

# RPMBar = tk.Label(window, bg= "green", width= 50, height= 4, font= ("Ubuntu", 10)) #Not writing anything, the height value scales with font size
RPMBar = tb.Floodgauge(window, bootstyle = "success",
                       maximum = 10000,
                       orient = "horizontal",
                       value = 0,
                       length = 1000)
Throttle = tk.Label(window, padx = 10, text = "0.0", bg = "#404040", fg = "white", font= ("Ubuntu", 30))

Speed = tk.Label(window, padx = 10, text = "1000", bg = "#404040", fg = "white", font= ("Ubuntu", 30))

ECUTemp = tk.Label(window, padx = 10, text = "10", bg = "#404040", fg = "white", font= ("Ubuntu", 30))



# Putting all the coordinates in defines would honestly be so much better
ECUTemp.place(x = 830, y = 775) # Position for ECU Temp 
Speed.place(x = 1510, y = 775) # Position for Vehicle speed
Battery.place(x = 425, y = 315)
RPM.place(x = 75, y = 315)
RPMBar.place(x = 20, y = 75)
# RPMBar.pack(pady = 75, padx = 20, fill = BOTH)
Throttle.place(x = 1200, y = 775)

# window.attributes("-fullscreen", True) # This one is kinda dangerous fr fr
window.after(50, update_func)
window.mainloop()
