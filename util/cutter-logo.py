# File: cutter-logo.py
#  A simple LOGO interpreter demo that can send stuff to libcutter for drawing/cutting
#
# Currently supported commands:
# FD, BK, LT, RT, PU, PD, REPEAT, SETX, SETY, SETPOS, SETH

from Tkinter import *
from ScrolledText import *
from turtle import *
import tkFileDialog
import tkMessageBox
from cutter import *

KEY2 = uint32Array(4)
KEY2[0] = 0x1
KEY2[1] = 0x2
KEY2[2] = 0x3
KEY2[3] = 0x4
KEY3 = uint32Array(4)
KEY3[0] = 0x5
KEY3[1] = 0x6
KEY3[2] = 0x7
KEY3[3] = 0x8

#adjust these as you see fit
render_width = 600
render_height = 300
render_border = 20

#scale_facter is the number to multiply by to go from cutter units to pixels
scale_factor = (render_width/(404.0*12.0))
#the height of the rendered mat is based on the width set above

pen_down = 0
my_cutter = C("/dev/ttyUSB0")
my_cutter.set_move_key(KEY2)
my_cutter.set_line_key(KEY3)

def draw_mat():
    render_window.create_rectangle(2, 2, render_width+2*render_border,\
                                   render_height+2*render_border,\
                                   fill="green")
    for i in range(13):
        render_window.create_line(i*((render_width)/12.0)+render_border, render_border/2,\
                                  i*((render_width)/12.0)+render_border, render_border)
        render_window.create_line(i*(render_width/12.0)+render_border, render_height+render_border,\
                                  i*(render_width/12.0)+render_border, render_height+render_border*1.5)
    for i in range(7):
        render_window.create_line(render_border/2, i*(render_height/6.0)+render_border,\
                                   render_border, i*(render_height/6.0)+render_border)
        render_window.create_line(render_width+render_border, i*(render_height/6.0)+render_border,\
                                   render_width+render_border*1.5, i*(render_height/6.0)+render_border)

def get_command_list(program):
    program = program.upper()               #upper case
    program = program.replace("\n", " ")    #remove new lines and tabs
    program = program.replace("\t", " ")
    program = program.replace("[", " [ ")   #add spaces around ]['s
    program = program.replace("]", " ] ")

    return program.split()                  #split

def eval_program(commands, simulate_on):
    global pen_down
    print "Evaluating Program:", commands

    #start popping commands off the list, evaluating them along the way
    while ( len(commands) > 0 ):
        move_command = FALSE
        command = commands.pop(0)
        if (command == "FD" or command == "FORWARD"):
            move_command = TRUE
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Forward ", int(parameter)
                my_turtle.fd(int(parameter)*scale_factor)
            else:
                tkMessageBox.showerror("Program Error", \
                                       "FORWARD command takes a single integer, given [%s]." % parameter)
                return FALSE

        elif (command == "BK" or command == "BACKWARD"):
            move_command = TRUE
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Backward ", int(parameter)
                my_turtle.bk(int(parameter)*scale_factor)
            else:
                tkMessageBox.showerror("Program Error", \
                                       "BACKWARD command takes a single integer, given [%s]." % parameter)
                return FALSE

        elif (command == "RT" or command == "RIGHT"):
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Rotate Right ", int(parameter)
                my_turtle.rt(int(parameter))
            else:
                tkMessageBox.showerror("Program Error", \
                                       "RIGHT command takes a single integer, given [%s]." % parameter)
                return FALSE

        elif (command == "LT" or command == "LEFT"):
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Rotate Left ", int(parameter)
                my_turtle.lt(int(parameter))
            else:
                tkMessageBox.showerror("Program Error", \
                                       "LEFT command takes a single integer, given [%s]." % parameter)
                return FALSE

        elif (command == "PU" or command == "PENUP"):
            pen_down = 0
            my_turtle.pu()
            print "Pen Up"

        elif (command == "PD" or command == "PENDOWN"):
            pen_down = 1
            my_turtle.pd()
            print "Pen Down"

        elif (command == "SETX"):
            move_command = TRUE
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Set x ", int(parameter)
                my_turtle.setx((int(parameter)*scale_factor) - render_width/2)
            else:
                tkMessageBox.showerror("Program Error", \
                                       "SETX command takes a single integer, given [%s]." % parameter)
                return FALSE

        elif (command == "SETY"):
            move_command = TRUE
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Set y ", int(parameter)
                my_turtle.sety((int(parameter)*scale_factor) - render_height/2)
            else:
                tkMessageBox.showerror("Program Error", \
                                       "SETY command takes a single integer, given [%s]." % parameter)
                return FALSE

        elif (command == "SETHEADING" or command == "SETH"):
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Set heading ", int(parameter)
                my_turtle.seth(int(parameter))
            else:
                tkMessageBox.showerror("Program Error", \
                                       "SETHEADING command takes a single integer, given [%s]." % parameter)
                return FALSE

        elif (command == "SETPOS" or command == "GOTO" or \
              command == "SETPOSITION" or command == "SETXY"):
            move_command = TRUE
            x = commands.pop(0)
            y = commands.pop(0)
            if (x.isdigit() and y.isdigit()):
                print "Set pos ", int(x), ",", int(y)
                my_turtle.setpos((int(x)*scale_factor) - render_width/2,\
                                 (int(y)*scale_factor) - render_height/2)
            else:
                tkMessageBox.showerror("Program Error", \
                                       "SETPOSITION command takes two integers, given [%s], [%s]." % (x, y))
                return FALSE

        elif (command == "REPEAT"):
            parameter = commands.pop(0)
            if (parameter.isdigit()):
                print "Repeat ", int(parameter)

                if (commands.pop(0) == "["):
                    repeat_commands = []
                    bracket_count = 0

                    #Put all commands in the repeat in another list and eval that
                    #the number of times specified.
                    while (len(commands) > 0):
                        repeat = commands.pop(0)
                        if (repeat == "["):
                            bracket_count = bracket_count + 1
                        elif (repeat == "]" and bracket_count == 0):
                            break;
                        elif (repeat == "]"):
                            bracket_count = bracket_count - 1

                        repeat_commands.append(repeat)

                    if (repeat != "]" or bracket_count != 0):
                        tkMessageBox.showerror("REPEAT Error", "REPEAT command improperly formatted.")
                        print "Bad terminating condition on REPEAT"
                        return FALSE
                    else:
                        for i in range(int(parameter)):
                            temp = repeat_commands[:]
                            if (eval_program(temp, simulate_on) == FALSE):
                                return FALSE
                else:
                    tkMessageBox.showerror("REPEAT Error", "REPEAT command improperly formatted.")
                    print "Bad formatting on REPEAT"
                    return FALSE
            else:
                return FALSE

        else:
            tkMessageBox.showerror("Unrecogized Command", "Command [%s] is not a know command." % command)
            print "Unrecogized command:", command
            return FALSE

        #Now do a bounds check and move the cutter if we're not simulating
        if (check_bounds() == FALSE):
            tkMessageBox.showerror("Out of bounds error", \
                                    "The turtle has left the mat, making this program un-cutable.")
            print "Turtle out of bounds!"
            return FALSE

        if (move_command and not simulate_on):
            if (cutter_cut() == FALSE):
                tkMessageBox.showerror("Cutter Error", \
                                    "An error occured when trying to talk to the cutter.")

    return TRUE

def check_bounds():
    print "Checking Bounds..."
    turtlex = my_turtle.xcor() + render_width/2
    turtley = my_turtle.ycor() + render_height/2
    if ((turtlex < 0) or (turtlex > render_width)):
        return FALSE
    elif ((turtley < 0) or (turtley > render_height)):
        return FALSE

    return TRUE

def cutter_cut():
    global pen_down
    print "Sending commands to cutter..."
    turtlex = (my_turtle.xcor() + render_width/2)/50.0
    turtley = (-my_turtle.ycor() + render_height/2)/50.0

    if (turtlex > 12 or turtley > 6):
        print "x: ",  turtley
        print "y: ", turtlex
    pt = xy()
    pt.y = turtlex
    pt.x = turtley

    if (pen_down == 1):
        my_cutter.cut_to(pt)
    else:
        my_cutter.move_to(pt)

def save_file():
    global code
    print "Saving..."
    save_file = tkFileDialog.asksaveasfile(mode='w')
    if save_file != None:
        text = str(code.get(0.0,END))
        save_file.write(text)
        save_file.close()

def load_file():
    global code
    print "Loading..."
    load_file = tkFileDialog.askopenfile(mode='r')
    if load_file != None:
        text = load_file.read()
        if text != None:
            code.delete(0.0, END)
            code.insert(END,text)
        load_file.close()

def simulate_cut():
    global pen_down
    print "Simulating..."

    #reinit everything
    pen_down = 0
    my_turtle.reset()
    my_turtle.penup()

    eval_program( get_command_list( code.get( '1.0', 'end' ) ), 1 )

def cut_on_cutter():
    global pen_down
    print "Cutting..."
    #reinit everything
    pen_down = 0
    my_turtle.reset()
    my_turtle.penup()

    my_cutter.start()

    pt = xy()
    pt.x=3.0
    pt.y=6.0
    my_cutter.move_to(pt)
    eval_program( get_command_list( code.get( '1.0', 'end' ) ), 0 )

    my_cutter.stop()

# main window
root = Tk()
root.title("libcutter Logo Interpreter")

#buttons
f = Frame(root, pady=5)
f.pack(side=TOP)

load_button = Button(f, text="Load", command=load_file, width=20)
load_button.pack(side=LEFT, expand=1, padx=5)

save_button = Button(f, text="Save", command=save_file, width=20)
save_button.pack(side=LEFT, expand=1, padx=5)

simulate_button = Button(f, text="Simulate", command=simulate_cut, width=20)
simulate_button.pack(side=LEFT, expand=1, padx=5)

cut_button = Button(f, text="Cut!", command=cut_on_cutter, width=20)
cut_button.pack(side=LEFT, expand=1, padx=5)

#label above board
w = Label(root, text="Cutter Mat Preview")
w.pack(side=TOP)

#canvas that shows mat
render_window = Canvas(root, width=(render_width+2*render_border), height=(render_height+2*render_border))
render_window.pack()
draw_mat()

#canvas for rendering turtle - placed inside mat canvas
turtle_canvas = Canvas(render_window, width=render_width, height=render_height, bd=0, highlightthickness=0)
turtle_canvas.place(x=render_border, y=render_border)
my_turtle = RawTurtle(turtle_canvas)

#label above code
w = Label(root, text="Logo Code")
w.pack(side=TOP)

#code entry field
code = ScrolledText(root, width=80, height=10)
code.pack(fill=BOTH, expand=1)

root.mainloop()
