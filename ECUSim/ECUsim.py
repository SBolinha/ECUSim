PID_Value_Map[PIDMemSize]
MCP_CAN CAN(10) # CAN CS: pin 10

def setup:
  Serial.begin(115200);
  print("------------------- ECUSim setup start ---------------------------")
  initializePIDValueMap()
  initializeCAN()

dev loop:
  if (Serial.available() >= SERIAL_MSG_LENGTH)
    parsePIDUPdateMessage()
  if (CAN.checkReceive() == CAN_MSGAVAIL)
    handleCANMessage()


