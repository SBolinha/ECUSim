import "PIDUpdateSerialControl.h"

flushSerialInbuf
parseOneByteHex(char *hexChars, byte *ret)

initializePIDValueMap:
  for i in range(i, PIDMemSize):
    PID_Value_Map[i] = 0;

flushSerialInbuf:
  if (PIDSET_ERROR)
    print("Flush serial port input buffer..")

  while (Serial.read() != -1):
    Serial.read();

  if (PIDSET_ERROR):
    print("Flush serial port input buffer finished.")

parsePIDUPdateMessage:
  if (PIDSET_DEBUG):
    print("PID update start.")

  serialBuf[SERIAL_MSG_LENGTH];

  # Store serial data to serialBuf.
  for i in range(0, SERIAL_MSG_LENGTH):
    serialBuf[i] = Serial.read();

  # Check delimiter
  if (serialBuf[SERIAL_MSG_LENGTH - 1] != '\n'):
    if (PIDSET_ERROR):
      print("ERROR: Input meassage is not finished with LF.")

    flushSerialInbuf()
    return

  # Parse serialBuf
  byte pidToSet;
  unsigned int pidMemAddress;
  byte valByteLength;
  byte valToSet[4];
  for i in range(0, SERIAL_MSG_LENGTH - 1, 2): # Ignore last byte of serialBuf (by setting i < SERIAL_MSG_LENGHT - 1, instead of i < SERIAL_MSG_LENGHT), since the last byte shold be LF.
    char parseBuf[2]
    parseBuf[0] = serialBuf[i]
    parseBuf[1] = serialBuf[i + 1]
    parsedVal = 0
    if (parseOneByteHex(parseBuf, parsedVal) != 0):
      if (PIDSET_ERROR):
        println("ERROR: Message parse is failed.")

      flushSerialInbuf()
      return
    # Store parsed result
    switch (i):
    case 0:
      pidToSet = parsedVal
      valByteLength = pgm_read_byte(PIDByteLengthMap + pidToSet)
      pidMemAddress = pgm_read_word(PIDAddressMap + pidToSet)
      if (PIDSET_DEBUG):
        print("PID:", pidToSet, HEX)
        print("ByteLength:", valByteLength, DEC)

      if (valByteLength == 0):
        if (PIDSET_ERROR)
          Serial.println(F("ERROR: Selected PID is not supported."));

        return
      break;
    default:
      valToSetIndex = int(i / 2 - 1)
      valToSet[valToSetIndex] = parsedVal
      break

  # Finally, update PID map
  for i in range(0, valByteLength):
    PID_Value_Map[pidMemAddress + i] = valToSet[i]

  if (PIDSET_DEBUG):
    print("SetVaule:", end = '')
    for i in range(0, valByteLength):
      print(PID_Value_Map[pidMemAddress + i], HEX, end = '')
      if (i != valByteLength - 1):
        print(F(","), end = '')
      else:
        print('')
    print("PID set Ok");

parseOneByteHex(hexChars, ret):
  val = 0;
  for i in range(0, 2):
    baseVal = 1 << 4 * (1 - i)
    if (hexChars[i] >= '0' && hexChars[i] <= '9'):
      val += (hexChars[i] - '0') * baseVal
    elif (hexChars[i] >= 'a' && hexChars[i] <= 'f'):
      val += (hexChars[i] - 'a' + 10) * baseVal
    elif (hexChars[i] >= 'A' && hexChars[i] <= 'F'):
      val += (hexChars[i] - 'A' + 10) * baseVal
    else:
      return -1

  ret = val
  return 0
