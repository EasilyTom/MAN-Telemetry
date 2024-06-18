import tkinter as tk
import tkinter.font as tkFont
from PIL import Image, ImageTk
from enum import Enum
import pandas as pd
import os

def read_forza():
    import forza_sim_data.data_parser as fsdp
    import pandas as pd
    
    __full_forza_data = []

    for _,data in fsdp.dataframe.iterrows():
        __ecu_param_list = [None, None, None, None, None, None, None, None, None, None, None] # Initing a global array 
        for _, keyblock in fsdp.key_list.iterrows():
            value = data[keyblock['corrected_position']]
            if pd.isna(keyblock['conversion']) == False:
                value = value * keyblock['conversion']
                
            if keyblock['name'] == 'Gear':
                __ecu_param_list[4] = int(value)
            elif keyblock['name'] == 'Speed':
                __ecu_param_list[5] = int(round(value, 0))
            elif keyblock['name'] == 'CurrentEngineRpm':
                __ecu_param_list[0] = value
            elif keyblock['name'] == 'Accel':
                __ecu_param_list[1] = int(round(value, 0))
        
        __full_forza_data.append(__ecu_param_list)
        # print(__ecu_param_list)

    return __full_forza_data

canPipe = "/tmp/can_data_pipe"
print("Display test. Reading: " + canPipe)
# os.system("./canusb -d dev/ttyUSB0 -s 1000000") # This can also run the cansub. Bash script not necessary

# 0: RPM, 1: Throttle, 2: ?, 3: Battery, 4: Gear, 5: Speed, 6: ECU Temp, 7: Coolant Temp, 8: Fuel Pressure, 9: Oil Pressure, 10: Oil Temp
# ecu_param_list = [None, None, None, None, None, None, None, None, None, None, None] # Initing a global array
ecu_param_list = ["0"] * 11

def try_reading(): 
   try:
      with open(canPipe, "r") as pipe:
         global ecu_param_list
         data_buffer = pipe.read() #Okay this is reading properly. Fucking sexy
         ecu_param_list = data_buffer.split(',')
         
        #  if len(ecu_param_list) < 11:
        #      ecu_param_list.extend([None] * (11 - len(ecu_param_list)))
        #  print(ecu_param_list)
        
         ECU_DATA.update_throttle_position(ecu_param_list[2])
         ECU_DATA.update_coolant_temp(ecu_param_list[6])
         ECU_DATA.update_ecu_temp(ecu_param_list[8])
         ECU_DATA.update_oil_temp(ecu_param_list[5])
         ECU_DATA.update_rpm(ecu_param_list[0])
        #  print(ECU_DATA.ecu_temp)
         
         # for x in ecu_param_list:
         #    print(x)
   except FileNotFoundError:
      print("Named pipe does not exist!")
   except OSError as e:
      print(f"Failed to read data from named pipe: {e}")

index = 0
# full_forza_data = read_forza()

class ECU_DataClass:
    def __init__(self):
        self.ecu_temp = 0.0
        self.coolant_temp = 0.0
        self.rpm = 0.0
        self.oil_temp = 0.0
        self.throttle_position = 0.0
        
    def update_ecu_temp(self, raw):
        self.ecu_temp = float(raw) / 10.0
    
    def update_coolant_temp(self, raw):
        self.coolant_temp = float(raw) - 273.0
        
    def update_rpm(self, raw):
        self.rpm = int(raw)
        
    def update_oil_temp(self, raw):
        self.oil_temp = float(raw) - 273.0
        
    def update_throttle_position(self, raw):
        self.throttle_position = int(raw)
        self.throttle_position = self.throttle_position / 10
        

# Field = (Title, Unit String, [Low, High])
class Fields(Enum):
    THROTTLE_POS = ("THROTTLE POSITION", "%", [20, 90])
    FUEL_PRES = ("FUEL PRESSURE", "kPa", [0, 100])
    OIL_PRES = ("OIL PRESSURE", "kPa", [0, 100])
    BATTERY_VOLT = ("BATTERY VOLTAGE", "V", [0, 100])
    ECU_TEMP = ("ECU TEMPERATURE", "C", [0, 100])
    COOLANT_TEMP = ("COOLANT TEMPERATURE", "C", [0, 100])
    OIL_TEMP = ("OIL TEMPERATURE", "C", [0, 100])
    RPM = ("RPM", "RPM", [0, 10000])
    
class StatusConfigs(Enum):
    # Status Text, Box Image Path, Param Title Text Colour, Status Text Colour, Value Colour
    NORMAL = ("OK", "box_images/box.png", "#B2B2B2", "#99FF75", "#FFFFFF")
    LOW = ("LOW", "box_images/low_box.png", "#9DBEFF", "#8DABE5", "#2871FF")
    HIGH = ("HIGH", "box_images/warning_box.png", "#FF5050", "#E5B9B9", "#FF5050")
    FUCKED = ("UNAVAIL", "box_images/warning_box.png", "#FF5050", "#E5B9B9", "#FF5050")

class MetricBox():
    def __init__(self, window, x, y, field: Fields):
        
        self.param_status = StatusConfigs.NORMAL
        self.field = field
        
        self.frame = tk.Frame(window, bd=0, highlightthickness=0, bg='black')
        self.frame.place(x=x, y=y)
        
        # Create the box image
        box_image = Image.open(self.param_status.value[1])
        box_image = box_image.resize((254, 70))
        box_image = box_image.convert("RGB")
        box_image = ImageTk.PhotoImage(box_image)
        self.background = tk.Label(self.frame, image=box_image, bd=0, highlightthickness=0)
        
        self.background.place()
        self.background.pack()
        
        self.background.image = box_image
        
        self.value_element = tk.Label(self.frame, text=f'?? {self.field.value[1]}', bg='black', fg=self.param_status.value[4], anchor='e')
        self.value_element.config(font=BJ30_SEMI)
        self.value_element.place(x=125, y=16, width=120, height=38)
        
        self.status_element = tk.Label(self.frame, text=self.param_status.value[0], bg='black', fg=self.param_status.value[3])
        self.status_element.config(font=BJ20_REG)
        self.status_element.place(x=9, y=27)
        
        self.title_element = tk.Label(self.frame, text=field.value[0], bg='black', fg=self.param_status.value[2])
        self.title_element.config(font=BJ10_REG)
        self.title_element.place(x=9, y=12)
        
    def update_box_image(self):
        box_image = Image.open(self.param_status.value[1])
        box_image = box_image.resize((254, 70))
        box_image = box_image.convert("RGB")
        box_image = ImageTk.PhotoImage(box_image)
        self.background = tk.Label(self.frame, image=box_image, bd=0, highlightthickness=0)
        
    def update_colouring(self):
        self.value_element.config(fg=self.param_status.value[4])
        self.status_element.config(text=self.param_status.value[0], fg=self.param_status.value[3])
        self.title_element.config(fg=self.param_status.value[2])
        
    def update_box(self, value):
        if value == None:
            self.param_status = StatusConfigs.FUCKED
            self.value_element.config(text=f'XX {self.field.value[1]}')
        elif value < self.field.value[2][0]:
            self.param_status = StatusConfigs.LOW
            self.value_element.config(text=f'{value} {self.field.value[1]}')
        elif value > self.field.value[2][1]:
            self.param_status = StatusConfigs.HIGH
            self.value_element.config(text=f'{value} {self.field.value[1]}')
        else:
            self.param_status = StatusConfigs.NORMAL
            self.value_element.config(text=f'{value} {self.field.value[1]}')
        
        self.update_box_image()
        self.update_colouring()
        
class ShiftLights():
    def __init__(self, window, x, y):
        self.frame = tk.Frame(window, bd=0, highlightthickness=0, bg='black')
        
        self.canvas = tk.Canvas(self.frame, bg='black', bd=0, highlightthickness=1, highlightbackground='#4C4C4C', width=722, height=39)
        self.canvas.pack()
        
        self.light_colors = [
            '#EC0000', '#EC0000', '#EC0000', '#EC0000', '#EC0000',
            '#99FF75', '#99FF75',
            '#2871FF',
            '#8C6FFF', '#8C6FFF'
        ]
        
        self.lights = []
        x_offset = 51
        for i in range(10):
            self.lights.append(self.canvas.create_oval(x_offset, 6, (x_offset + 26), (6+26), fill='#4C4C4C', outline='#4C4C4C'))
            x_offset += (26+40)
            
        self.boost_bar_image = Image.open('box_images/boost_bar.png')
        self.boost_bar_image = self.boost_bar_image.resize((30, 480))
        self.boost_bar_image = ImageTk.PhotoImage(self.boost_bar_image)

        self.left_bar = tk.Label(window, image=self.boost_bar_image, width=30, height=480)
        self.right_bar = tk.Label(window, image=self.boost_bar_image, width=30, height=480)
        
        self.frame.place(x=x, y=y, width=722, height=39)
        
    def update_lights(self, rpm):
        # Reset all lights to off
        for light in self.lights:
            self.canvas.itemconfig(light, fill='#4C4C4C', outline='#4C4C4C')
        
        self.left_bar.place_forget()
        self.right_bar.place_forget()

        # Determine how many lights to turn on
        if rpm > 8000:
            lights_to_illuminate = 10
            self.left_bar.place(x=0, y=0)
            self.right_bar.place(x=770, y=0)
        elif rpm > 7000:
            lights_to_illuminate = 8
        else:
            # Linearly scale the number of lights based on the rpm value
            lights_to_illuminate = int(rpm / 8000 * 10)

        # Turn on the appropriate number of lights
        for i in range(lights_to_illuminate):
            self.canvas.itemconfig(self.lights[i], fill=self.light_colors[i], outline=self.light_colors[i])
            
def close_app(event):
    if event.char.lower() == 'q':
        window.destroy()
        
ECU_DATA = ECU_DataClass()
        
window = tk.Tk(className="Dash V2")
window.geometry("800x480")
window.configure(bg='black')

window.bind('<q>', close_app)

font_scale = 0.7 # 0.7 for the PI
BJ10_REG = tkFont.Font(family='Bai Jamjuree', size=int(round(10*font_scale, 0)))
BJ20_REG = tkFont.Font(family='Bai Jamjuree', size=int(round(20*font_scale, 0)))
BJ30_SEMI = tkFont.Font(family='Bai Jamjuree', size=int(round(30*font_scale, 0)), weight='bold')
BJ200_BOLD = tkFont.Font(family='Bai Jamjuree', size=int(round(200*font_scale, 0)), weight='bold')
BJ50_BOLD = tkFont.Font(family='Bai Jamjuree', size=int(round(50*font_scale, 0)), weight='bold')

skeleton = Image.open('skeleton.png')
skeleton = skeleton.resize((800, 480))
skeleton = ImageTk.PhotoImage(skeleton)

label1 = tk.Label(window, image = skeleton)
label1.place(x=0, y=0)

# frameCnt = 76
# frames = [tk.PhotoImage(file='box_images/plaid.gif',format = 'gif -index %i' %(i)) for i in range(frameCnt)]
# ind = 0

# def update_gif(ind):
#     frame = frames[ind]
#     ind += 1
#     if ind == frameCnt:
#         ind = 0
#     gif_player.configure(image=frame)
#     window.after(20, update_gif, ind)

# gif_player = tk.Label(window, bg='black', width=800, height=450)
# playing_gif = False

gear_indicator = tk.Label(window, text='N', bg='black', fg='white')
gear_indicator.config(font=BJ200_BOLD)
gear_indicator.place(x=337, y=87, width=125, height=200)

speed_indicator = tk.Label(window, text='XXX', bg='black', fg='white')
speed_indicator.config(font=BJ50_BOLD)
speed_indicator.place(x=354, y=344, width=90, height=63)

boxes = []

boxes.append(MetricBox(window, 38, 67, field=Fields.FUEL_PRES))
boxes.append(MetricBox(window, 38, 152, field=Fields.OIL_PRES))
boxes.append(MetricBox(window, 38, 237, field=Fields.THROTTLE_POS))
boxes.append(MetricBox(window, 38, 322, field=Fields.RPM))

boxes.append(MetricBox(window, 508, 67, field=Fields.BATTERY_VOLT))
boxes.append(MetricBox(window, 508, 152, field=Fields.ECU_TEMP))
boxes.append(MetricBox(window, 508, 237, field=Fields.COOLANT_TEMP))
boxes.append(MetricBox(window, 508, 322, field=Fields.OIL_TEMP))

for box in boxes:
    box.update_box(None)

shift_lights = ShiftLights(window, 39, 9)

def update_func(play_forza=False):
    global full_forza_data
    global index
    # global gif_player
    # global playing_gif
    
    if play_forza is True:
        ecu_param_list = full_forza_data[index]
        index+=1
    else:
        ecu_param_list = [0] * 11
        try_reading()
    
    # rpm = int(ecu_param_list[0])
    speed = int(ecu_param_list[5])
    
    # if speed >= 70 and speed <= 150:
    #     if playing_gif is False:
    #         playing_gif = True
    #         gif_player.place(x=0, y=0)
            
        # window.after(1, update_gif, ind)
    # else:
    #     playing_gif = False
    #     gif_player.place_forget()
    
    gear = ecu_param_list[4]
    
    gear_indicator.config(text=gear)
    speed_indicator.config(text=speed)
    
    for box in boxes:
        if (box.field == Fields.THROTTLE_POS):
            box.update_box(value=ECU_DATA.throttle_position)
        elif (box.field == Fields.BATTERY_VOLT):
            box.update_box(value=ecu_param_list[3])
        elif (box.field == Fields.ECU_TEMP):
            box.update_box(value=ECU_DATA.ecu_temp)
        elif (box.field == Fields.COOLANT_TEMP):
            box.update_box(value=ECU_DATA.coolant_temp)
        elif (box.field == Fields.FUEL_PRES):
            box.update_box(value=ecu_param_list[8])
        elif (box.field == Fields.OIL_PRES):
            box.update_box(value=ecu_param_list[9])
        elif (box.field == Fields.RPM):
            box.update_box(value=ECU_DATA.rpm)
        elif (box.field == Fields.OIL_TEMP):
            box.update_box(value=ECU_DATA.oil_temp)
        else:
            continue
        
    shift_lights.update_lights(int(ECU_DATA.rpm))
    
    window.after(1, update_func)

window.attributes("-fullscreen", True)
window.after(1, update_func)
# window.after(1, update_gif, ind)
window.mainloop()