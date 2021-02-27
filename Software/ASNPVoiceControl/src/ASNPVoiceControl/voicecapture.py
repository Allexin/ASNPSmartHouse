#!/usr/bin/env python3

from vosk import Model, KaldiRecognizer
import os
import sys
import io

sys.stdout.reconfigure(encoding='utf-8')

if not os.path.exists("model"):
    print ("Please download the model from https://alphacephei.com/vosk/models and unpack as 'model' in the current folder.")
    exit (1)
    
#f = io.open("dictionary.txt", mode="r", encoding="utf-8")    
#data = f.read()

import pyaudio

model = Model("model")
rec = KaldiRecognizer(model, 16000)

p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paInt16, channels=1, rate=16000, input=True, frames_per_buffer=8000)
stream.start_stream()

while True:
    data = stream.read(4000)
    if len(data) == 0:
        break
    if rec.AcceptWaveform(data):
        print(rec.Result(), flush=True)

print(rec.FinalResult())
