import speech_recognition as sr #3.10.1
import pyttsx3 #2.90
import openai
import time
import datetime
import serial
import tkinter as tk
from tkinter import scrolledtext
import requests

url="http://192.168.96.149/26/on"
url2="http://192.168.96.149/26/off"
#x = requests.get(url)


light_state=0

ser=serial.Serial(port='/dev/ttyUSB0',baudrate=115200,timeout=1)
#[mode_selection,movement,angle to turn,duration_of_movement,RESPONSE OF THE BOT]
root = tk.Tk()
root.title("RC Car Control Interface")
root.geometry("480x320")

alarms=[]
mess_his = [{"role":"system","content":"you are a rc car with bluetooth and voice controls with voice assistant features you can also give voice assistant results  is asked  i want you to return a list(but instead of commas use | ) with 4 elements(0-4) element 0 being the mode (3 modes)selection ie bluetooth=0,voicecontrol=1  and iotcontrol=2 the default selection is voice control...element 1 being the movement of wheels ie 1=foward,0=still,-1=reverse by default uless foward or front is mentioned the value will be 0 the vehicle will be still... element 2 is the angle to turn if 0=still,+ve no upto 180 = turn right,-ve no upto -180=turn left by default the value is 0 and while an angle is mentioned the wheels must always be 0....element 3 is the duration of the foward and reverse ie 20 = 20 seconds foward or reverse depending on the direction of the wheels if duration is not mentioned the duration is 0 which means forever here when duration is given angle is always 0... finally the element 4 is the voice assistant response example (turning 37 degrees right etc) or if a general voice assistant question or information (any information can be given so feel free to give out any information but make it consise but informative )is asked continue the previous movement result and give the change response to whatever is being asked in element 4  if bluetooth mode is given set all other elements to 0 (the constraints are only 4 elements are allowed to be returned , time must not be returned at any cost unless it is specificalyy asked for ...if iot control( controlling of light etc) is asked set element 0 to '2' (lights on etc)  and  elements 1,2  = '0' and element 3 will have the voice assistant response ie('lights tuened on etc' ) and remember only 4 elements (0,3)allowed)"}]
openai.api_key='s'
engine = pyttsx3.init(driverName='espeak')
voices = engine.getProperty('voices')
engine.setProperty('voice', voices[11].id)
engine.say("HELLO BOSS ...i am your assistant ...jarvis!")
engine.runAndWait()
voice_assistant_name="jarvis"

def info_action():
    response_display.insert(tk.END, "Team The Noble Knights\n1.Steve Samuel\n2.Vignesh\n3.Tamil\n4.Varun\n5.Sangeeth\n\n")
    response_display.see(tk.END)  # Auto-scroll to the end

def light_action():
    global light_state
    response_display.insert(tk.END, "Light Toggle Button Pressed\n\n")
    response_display.see(tk.END)  # Auto-scroll to the end
    if light_state==0:
        x=requests.get(url)
        light_state=1
    else:
        y=requests.get(url2)

def btmode_action():
    response_display.insert(tk.END, "Bluetooth Mode Activated\n\n")
    response_display.see(tk.END)  # Auto-scroll to the end
    ser.write(b'0 0 0 0')
    
    
def takeCommand():
    r=sr.Recognizer()
    with sr.Microphone() as source:
        r.adjust_for_ambient_noise(source)
        print("Listening...")
        root.update()
        audio=r.listen(source,phrase_time_limit=3)

    try:
        statement = r.recognize_google(audio, language='en-in').lower()
        return statement
    #    engine.say(statement)
   #     engine.runAndWait()
   #     print(f"user said:{statement}\n")

    except Exception as e:
        return("none")
    return statement

def compute(statement):
    global light_state
    print(statement)
    if voice_assistant_name.lower() in statement and 'are you working' in statement:
        engine.say("Yes boss ... I am working")
        engine.runAndWait()
    elif voice_assistant_name.lower() in statement and 'time' in statement and 'now' in statement:
        t = time.localtime()
        current_time = time.strftime("%H:%M:%S", t) 
        print(current_time)                
        engine.say(current_time)
        engine.runAndWait()
    elif voice_assistant_name.lower() in statement:
        request , gpt_response= ask_openai(statement)
        engine.say(gpt_response)
        engine.runAndWait()
        print(request)
        print(gpt_response)
    else:
        if statement=='none':
            root.update()
            pass
        else:
            request , gpt_response,esp_data= ask_openai(statement)
            engine.say(gpt_response)
            engine.runAndWait()
            print(gpt_response)
            response_display.insert(tk.END,gpt_response+"\n")
            response_display.see(tk.END)  # Auto-scroll to the end
            root.update()
            if request[0]=='2':
                if light_state==0:
                    x=requests.get(url)
                    light_state=1
                else:
                    y=requests.get(url2)

            x=""
            for i in esp_data:
                x+=i
                x+=" "
            esp_data=x
            print(esp_data)
            encoded_data=str(esp_data).encode('utf-8')
            ser.write(encoded_data)
            



def ask_openai(question):
    global mess_his
    now = str(datetime.datetime.now())
    question=question +"...current time is "+ now +" in india strictly dont return time unless it is asked for"
    mess_his.append({"role": "user", "content": question})
    completion = openai.ChatCompletion.create(
        model="gpt-3.5-turbo",#"gpt-3.5-turbo"
        messages=mess_his
    )
    gpt_response = completion.choices[0].message.content
    mess_his.append({"role": "assistant", "content": gpt_response})
    print("GPT:",now,":",gpt_response)
    response=list(map(str,gpt_response.split("|")))
    esp_data=response[0:-1]

    return gpt_response,response[-1],esp_data
        # Create frames for layout
left_frame = tk.Frame(root, width=160, height=320)
left_frame.pack_propagate(False)  # Prevents frame from resizing
left_frame.pack(side=tk.LEFT, fill=tk.BOTH)

right_frame = tk.Frame(root, width=320, height=320)
right_frame.pack_propagate(False)
right_frame.pack(side=tk.RIGHT, fill=tk.BOTH)

# Left frame buttons
btn_info = tk.Button(left_frame, text="Info", command=info_action)
btn_info.pack(fill=tk.BOTH, expand=True)

btn_light = tk.Button(left_frame, text="Light On/Off", command=light_action)
btn_light.pack(fill=tk.BOTH, expand=True)

btn_btmode = tk.Button(left_frame, text="BT Mode", command=btmode_action)
btn_btmode.pack(fill=tk.BOTH, expand=True)

# Right frame for voice assistant response
response_display = scrolledtext.ScrolledText(right_frame, wrap=tk.WORD, font=("Arial", 12))
response_display.pack(expand=True, fill=tk.BOTH)
root.update()
if __name__=='__main__':
    while True:
        # Get voice command within the Tkinter main loop
        statement = takeCommand().lower()
        compute(statement)
        root.update()  # Update the Tkinter window

        # Run the Tkinter main loo

            

