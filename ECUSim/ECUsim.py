PID_Value_Map[PIDMemSize]
MCP_CAN CAN(10) # CAN CS: pin 10

setup():
  Serial.begin(115200);
  print("------------------- ECUSim setup start ---------------------------")
  initializePIDValueMap()
  initializeCAN()

void loop():
  if (Serial.available() >= SERIAL_MSG_LENGTH)
    parsePIDUPdateMessage()
  if (CAN.checkReceive() == CAN_MSGAVAIL)
    handleCANMessage()


