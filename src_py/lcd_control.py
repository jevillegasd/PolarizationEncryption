import sys
if sys.version_info[0] == 2:  # the tkinter library changed it's name from Python 2 to 3.
    import Tkinter
    tkinter = Tkinter #I decided to use a library reference to avoid potential naming conflicts with people's programs.
else:
    import tkinter
from PIL import Image, ImageTk  
from screeninfo import get_monitors
import time


PROJ_PATH = "C:\\Users\\yoj20\\Documents\\A_PROJECTS\\A_a3DProo\\vs_project\\HackCreality\\data\\layers"

monitors = []
for m in get_monitors():
    monitors.append(m)

w1 = monitors[0].width
h1 = monitors[0].height
w2 = monitors[1].width
h2 = monitors[1].height

print(w1, h1, w2, h2)


def display_on_lcd(layer_img):
    root = tkinter.Toplevel()
    root.overrideredirect(1)
    root.geometry("%dx%d+%d+%d" % (w2, h2, w1 , 0)) 
    root.focus_set()    
    root.bind("<Escape>", lambda e: (e.widget.withdraw(), e.widget.quit()))
    canvas = tkinter.Canvas(root, width=w2, height=h2)
    canvas.pack()
    canvas.configure(background='black')
    image = ImageTk.PhotoImage(layer_img)
    canvas.create_image(w2/2, h2/2, image=image) 
    root.mainloop() 

# layer_image = Image.open("t_img_2560.png")
layer_image = Image.open("C:\\Users\\yoj20\\Documents\\A_PROJECTS\\A_a3DProo\\vs_project\\HackCreality\\data\\layers\\layer1.bmp") 
display_on_lcd(layer_image) 

def automate_print():
    t_first_10 = 57
    t_other_layers = 24
    no_layers = 5
    no_repeat = 20
    abs_layer = 0
    for l in range(no_layers):
        layer_image = Image.open(PROJ_PATH + "\\layer" + str(l+1) + "scaled.bmp") 
        display_on_lcd(layer_image)
        for i in range(no_repeat):
            start_time = time.perf_counter()

            if i < 10 and l == 0:
                while time.perf_counter() - start_time < t_first_10:
                    continue
            else:
                while time.perf_counter() - start_time < t_other_layers:
                    continue
            abs_layer += 1
            print("Current image layer: ", l) 
            print("Currrent absolute layer: ", abs_layer)



#automate_print()

