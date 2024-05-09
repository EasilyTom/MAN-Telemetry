import tkinter as tk
from PIL import Image, ImageTk

class MetricBox():
    def __init__(self, window, x, y):
        
        self.frame = tk.Frame(window, bd=0, highlightthickness=0)
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
        

window = tk.Tk(className="Dash V2")
window.geometry("800x480")
window.configure(bg='black')

skeleton = Image.open('skeleton.png')
skeleton = skeleton.resize((800, 480))
skeleton = ImageTk.PhotoImage(skeleton)

label1 = tk.Label(window, image = skeleton)
label1.place(x=0, y=0)

def draw_initial_boxes():
    boxes = []
    boxes.append(MetricBox(window, 38, 67))
    boxes.append(MetricBox(window, 38, 152))
    boxes.append(MetricBox(window, 38, 237))
    
    boxes.append(MetricBox(window, 508, 67))
    boxes.append(MetricBox(window, 508, 152))
    boxes.append(MetricBox(window, 508, 237))
    
draw_initial_boxes()
window.mainloop()