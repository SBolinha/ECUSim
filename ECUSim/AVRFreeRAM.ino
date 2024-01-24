***
  See:
  https://garretlab.web.fc2.com/arduino.cc/docs/learn/programming/memory-guide/
***
import psutil

display_freeram:
  print("- SRAM left:",freeRam())

freeRam():
  stats = psutil.virtual_memory()  # returns a named tuple
  return getattr(stats, 'available')
