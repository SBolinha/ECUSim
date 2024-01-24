import "CANMesasgeHandle.h"
import "PIDMessageBuilder.h"
import "AVRFreeRAM.h"

IsoTp isotp(CAN, 0)

int RETURN_MSGBUILD_BUF_LENGTH = 128
int PID_LIST_LENGTH = 6

initializeCAN:
  boolinitSucess = False
  while (!initSucess):
    if (CAN_OK == CAN.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ)): # init can bus : baudrate = 250k
      println("CAN BUS Shield init ok!")
      initSucess = True
      CAN.setMode(MCP_NORMAL)
    else:
      print("CAN BUS Shield init fail")
      print("Init CAN BUS Shield again")
      delay(100)
      initSucess = False

handleCANMessage:
  canMsgHandleStartTime = 0
  if (CANMSG_TIME_MEAS):
    canMsgHandleStartTime = micros()

  if (CANMSG_DEBUG):
    print("CAN message handle start.")

  byte receivedCANBuf[CAN_PAYLOAD_LENGTH]
  unsigned long canId
  unsigned char len
  uint8_t recvResult

  recvResult = CAN.readMsgBuf(canId, &len, receivedCANBuf)

  if (len > CAN_PAYLOAD_LENGTH):
    if (CANMSG_FATAL):
      print("FATAL: CAN read message length exceed CAN_PAYLOAD_LENGTH.")
    return

  if(CANMSG_ERROR):
    if(recvResult != CAN_OK):
      print("ERROR: CAN message receive is failed. Return of readMsgBuf :", recvResult)

  if (CANMSG_DEBUG):
    print("MCP read result code:", recvResult)
    print("Msg from canId:", canId, HEX)
    print("Msg length:", len)
    print("Msg content:", end='')
    for i in range(0, CAN_PAYLOAD_LENGTH):
      print(receivedCANBuf[i], HEX)
      if (i == CAN_PAYLOAD_LENGTH - 1)
        print('')
      else
        print(",", end='')

  # Ignore query if the ID do not match with this ECU ID (or 0x7DF(send to all ECU))
  if((canId != 0x7DF) and (canId != ECU_CAN_ID)):
    if (CANMSG_DEBUG):
      print("CAM ID do not match with this ECU's ID.")
    return

  # Wait time
  if(ECU_WAIT > 0):
    delay(ECU_WAIT)

  # Get query message length and check service mode.
  queryMessageLength = receivedCANBuf[0]
  serviceMode = receivedCANBuf[1]
  if (serviceMode != 0x01):
    if (CANMSG_ERROR):
      print("ERROR: CAN query service mode needs to be 1 (show current data).")
    return

  if(queryMessageLength < 2 or queryMessageLength > 7):
    if (CANMSG_ERROR):
      print("ERROR: CAN query message length needs to be between 2 and 7 (1 to 6 PIDs).")
    return

  # Get query PID codes
  requestedPIDList[PID_LIST_LENGTH]
  uint8_t requestedPIDCount = queryMessageLength - 1 # Exclude service mode from query length
  for i in range(0, requestedPIDCount):
    requestedPIDList[i] = receivedCANBuf[i + 2]

  if (CANMSG_DEBUG):
    print("PID query: ", end = '')
    for i in range(0, requestedPIDCount):
      print(requestedPIDList[i], HEX, end = '')
      if(i == requestedPIDCount - 1)
        print('')
      else
        print(",", end = '')

  # Build up CAN return message
  returnServiceMode = serviceMode + 0x40
  returnMessageBuf[RETURN_MSGBUILD_BUF_LENGTH]
  returnByteCount;

  pidValMessageResult = buildPIDValueMessage(returnMessageBuf, returnByteCount, requestedPIDList, requestedPIDCount, returnServiceMode);
  if (pidValMessageResult == PID_NOT_AVAILABLE):
    if (CANMSG_ERROR):
      print("ERROR: CAN query PID is not supported.")
    return

  # Send CAN return message.
  struct Message_t txMsg;
  uint8_t sendResult;
  txMsg.len = returnByteCount;
  txMsg.rx_id = ECU_CAN_ID;
  txMsg.tx_id = ECU_CAN_RESPONSE_ID;
  txMsg.Buffer = returnMessageBuf;
  sendResult = isotp.send(&txMsg);

  if(CANMSG_ERROR):
    if(sendResult != CAN_OK):
      print("ERROR: CAN message send is failed. Return of sendMsg :", sendResult)

  if(CANMSG_TIME_MEAS):
    Serial.print("CAN message handle time (micros):", micros() - canMsgHandleStartTime)

  if (CANMSG_DEBUG):
    print("Return. Value (with padding):", end = '')
    for i in range(0, returnByteCount):
      print(returnMessageBuf[i], HEX, end = '');
      if (i == returnByteCount - 1):
        print('')
      else:
        print(",", end = '')

    print("MCP send result code :", sendResult)

  if(CANMSG_FREERAM_MEAS):
    display_freeram()
