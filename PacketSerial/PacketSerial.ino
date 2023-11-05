#include <PacketSerial.h>
#include <CRC.h>

// Types
#define BUTTON_REPORT_MSG 0x4252 // 'BR'
struct ButtonReportMsg {
  uint16_t messageType;
  uint16_t sequence;
  uint16_t controlMsgRecvCount;
  uint16_t buttonState;
  int16_t encoder0Value;
  int16_t encoder1Value;
};

#define BUTTON_CONTROL_MSG 0x4243 // 'BC'
struct ButtonControlMsg {
  uint16_t messageType;
  uint16_t sequence;
  uint16_t reportMsgRecvCount;
  uint16_t buttonLeds;
  uint16_t flags;
};

#define FLAGS_RESET_ENCODER_0 0x1
#define FLAGS_RESET_ENCODER_1 0x2

// Helpers
#define BYTE_SWAP_16(data) \
  ((((data) >> 8) & 0x00FF) | (((data) << 8) & 0xFF00)) 

#define BYTE_SWAP_32(data) \
  ( (((data) >> 24) & 0x000000FF) | (((data) >>  8) & 0x0000FF00) | \
    (((data) <<  8) & 0x00FF0000) | (((data) << 24) & 0xFF000000) )

void ByteSwap(ButtonReportMsg *msg) {
  msg->messageType = BYTE_SWAP_16(msg->messageType);
  msg->sequence = BYTE_SWAP_16(msg->sequence);
  msg->controlMsgRecvCount = BYTE_SWAP_16(msg->controlMsgRecvCount);
  msg->buttonState = BYTE_SWAP_16(msg->buttonState);
  msg->encoder0Value = BYTE_SWAP_16(msg->encoder0Value);
  msg->encoder1Value = BYTE_SWAP_16(msg->encoder1Value);
}

void ByteSwap(ButtonControlMsg *msg) {
  msg->messageType = BYTE_SWAP_16(msg->messageType);
  msg->sequence = BYTE_SWAP_16(msg->sequence);
  msg->reportMsgRecvCount = BYTE_SWAP_16(msg->reportMsgRecvCount);
  msg->buttonLeds = BYTE_SWAP_16(msg->buttonLeds);
  msg->flags = BYTE_SWAP_16(msg->flags);
}

// State
PacketSerial cobsSerial;
ButtonControlMsg controlMsg = {};
uint16_t controlMsgRecvCount = 0;
uint16_t reportMsgSequence = 0;

void setup()
{
  cobsSerial.begin(115200);
  cobsSerial.setPacketHandler(&onPacketReceived);
}

void loop()
{
  // Update serial and decode any message(s). If a ButtonControlMsg is
  // recieved then this count will be updated.
  uint16_t currentControlMsgCount = controlMsgRecvCount;
  cobsSerial.update();
  if (cobsSerial.overflow()) {
    // byte(s) slipped: increment counter
  }

  // If an ButtonControlMsg was received then the latest will be in controlMsg.
  if (currentControlMsgCount != controlMsgRecvCount) {
    processControlMsg(&controlMsg);
  }

  // Update the button report (read sensors, etc.).
  ButtonReportMsg reportMsg;
  buildReportMsg(&reportMsg);

  // Prepare packet with message and crc.
  uint8_t buffer[sizeof(ButtonReportMsg) + sizeof(uint16_t)] = {};
 
  // Get msg in network byte order and stuff into buffer.
  ByteSwap(&reportMsg);
  memcpy(buffer, &reportMsg, sizeof(ButtonReportMsg));

  // Calculate and stuff in msg CRC16.
  uint16_t crc16 = calcCRC16(buffer, sizeof(ButtonReportMsg));
  crc16 = BYTE_SWAP_16(crc16);
  memcpy(&buffer[sizeof(ButtonReportMsg)], &crc16, sizeof(uint16_t));

  // Send the report.
  cobsSerial.send(buffer, sizeof(buffer));

  // target ~50hz
  delay(20);
}

void buildReportMsg(ButtonReportMsg *msg) {
  // TODO
  msg->messageType = BUTTON_REPORT_MSG;
  msg->sequence = reportMsgSequence++;
  msg->controlMsgRecvCount = controlMsgRecvCount;
  msg->buttonState = 0;
  msg->encoder0Value = 0;
  msg->encoder1Value = 0;
}

void processControlMsg(const ButtonControlMsg *msg) {
  // TODO
  // msg->messageType
  // msg->sequence
  // msg->reportMsgRecvCount
  // msg->buttonLeds
  // msg->flags
}

void onButtonControlMsg(const ButtonControlMsg *msg) {
  memcpy(&controlMsg, msg, sizeof(ButtonControlMsg));
  controlMsgRecvCount++;
}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  // Confirm packet contains only ButtonControlMsg + CRC16 footer.
  if (size != sizeof(ButtonControlMsg) + sizeof(uint16_t)) {
    // incorrect packet size: increment counter
    return;
  }

  // Read and byteswap the message CRC16.
  uint16_t messageCrc16 = 0;
  memcpy(&messageCrc16, &buffer[sizeof(ButtonControlMsg)], sizeof(uint16_t));
  messageCrc16 = BYTE_SWAP_16(messageCrc16);

  // Compute and compare CRC16 of message
  uint16_t crc16 = calcCRC16(buffer, sizeof(ButtonControlMsg));
  if (messageCrc16 != crc16) {
    // incorrect crc: increment counter
    return;
  }

  ButtonControlMsg msg = {};

  // Read in and byte swap the message
  memcpy(&msg, buffer, sizeof(ButtonControlMsg));
  ByteSwap(&msg);
  
  // Only handles ButtonControlMsg's
  if (controlMsg.messageType != BUTTON_CONTROL_MSG) {
    // incorrect messageType: increment counter
  }

  // Handle this message
  onButtonControlMsg(&msg);
}
