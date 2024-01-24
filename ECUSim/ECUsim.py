# ECU (this controller) CAN ID
ECU_CAN_ID = 0x7E0;
ECU_CAN_RESPONSE_ID = ECU_CAN_ID + 0x008;

# ECU response(reply wait in ms)
ECU_WAIT = 0; 

# Debug message serial out switch
PIDSET_DEBUG = False;
PIDSET_ERROR = True;
CANMSG_DEBUG = False;
CANMSG_TIME_MEAS = False;
CANMSG_FREERAM_MEAS = False;
CANMSG_ERROR = True;
CANMSG_FATAL = True;

SERIAL_MSG_LENGTH = 11;
CAN_PAYLOAD_LENGTH = 8;

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


