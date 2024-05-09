import tkinter as tk
from PIL import Image, ImageTk
from enum import Enum

class Fields(Enum):
    THROTTLE_POS = ("THROTTLE POSITION", "%")
    FUEL_PRES = ("FUEL PRESSURE", "kPa")
    OIL_PRES = ("OIL PRESSURE", "kPa")
    BATTERY_VOLT = ("BATTERY VOLTAGE", "V")
    ECU_TEMP = ("ECU TEMPERATURE", "C")
    COOLANT_TEMP = ("COOLANT TEMPERATURE", "C")
    
class StatusConfigs(Enum):
    # Status Text, Box Image Path, Param Title Text Colour, Status Text Colour, Value Colour
    NORMAL = ("OK", "", "#B2B2B2", "#99FF75", "#FFFFFF")
    LOW = ("LOW", "", "#9DBEFF", "#8DABE5", "#2871FF")
    HIGH = ("HIGH", "", "#FF5050", "#E5B9B9", "#FF5050")

class MetricBox():
    def __init__(self, window, x, y, field: Fields):
        
        self.param_status = StatusConfigs.NORMAL
        self.field = field
        
        self.frame = tk.Frame(window, bd=0, highlightthickness=0, bg='black')
        self.frame.place(x=x, y=y)
        
        # Create the box image
        box_image = Image.open('box_images/box.png')
        box_image = box_image.resize((254, 70))
        box_image = box_image.convert("RGB")
        box_image = ImageTk.PhotoImage(box_image)
        self.background = tk.Label(self.frame, image=box_image, bd=0, highlightthickness=0)
        
        self.background.place()
        self.background.pack()
        
        self.background.image = box_image
        
        self.value_element = tk.Label(self.frame, text=f'?? {self.field.value[1]}', bg='black', fg=self.param_status.value[4], anchor='e')
        self.value_element.config(font=("Bai Jamjuree", 30))
        self.value_element.place(x=125, y=16, width=120, height=38)
        
        self.status_element = tk.Label(self.frame, text=self.param_status.value[0], bg='black', fg=self.param_status.value[3])
        self.status_element.config(font=("Bai Jamjuree", 20))
        self.status_element.place(x=9, y=27)
        
        self.title_element = tk.Label(self.frame, text=field.value[0], bg='black', fg=self.param_status.value[2])
        self.title_element.config(font=("Bai Jamjuree", 10))
        self.title_element.place(x=9, y=12)
        

window = tk.Tk(className="Dash V2")
window.geometry("800x480")
window.configure(bg='black')

skeleton = Image.open('skeleton.png')
skeleton = skeleton.resize((800, 480))
skeleton = ImageTk.PhotoImage(skeleton)

label1 = tk.Label(window, image = skeleton)
label1.place(x=0, y=0)

gear_indicator = tk.Label(window, text='N', bg='black', fg='white')
gear_indicator.config(font=("Bai Jamjuree", 200))
gear_indicator.place(x=337, y=87, width=125, height=200)

speed_indicator = tk.Label(window, text='XXX', bg='black', fg='white')
speed_indicator.config(font=("Bai Jamjuree", 50))
speed_indicator.place(x=354, y=344, width=90, height=63)

def draw_initial_boxes():
    boxes = []
    boxes.append(MetricBox(window, 38, 67, field=Fields.FUEL_PRES))
    boxes.append(MetricBox(window, 38, 152, field=Fields.OIL_PRES))
    boxes.append(MetricBox(window, 38, 237, field=Fields.THROTTLE_POS))
    
    boxes.append(MetricBox(window, 508, 67, field=Fields.BATTERY_VOLT))
    boxes.append(MetricBox(window, 508, 152, field=Fields.ECU_TEMP))
    boxes.append(MetricBox(window, 508, 237, field=Fields.COOLANT_TEMP))
    
draw_initial_boxes()
window.mainloop()