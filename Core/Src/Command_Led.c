#include "Commands.h"
#include "UARTStream.h"
#include "main.h"

static const char CMD_LED_NAME[] = "led";

typedef struct {
  GPIO_TypeDef*       GPIO;
  uint16_t            Pin;
} Led_PinConfig;

static const Led_PinConfig LEDS[] = {
  {LED_GREEN_GPIO_Port, LED_GREEN_Pin},
  {LED_RED_GPIO_Port, LED_RED_Pin},
};

static Cmd_Handled Led_onSet(CmdManager* manager, Cmd* cmd, Param_Cursor* cursor, Cmd_Type type);
static Cmd_Handled Led_onGet(CmdManager* manager, Cmd* cmd, Param_Cursor* cursor, Cmd_Type type);
static Cmd_Handled Led_onHelp(CmdManager* manager, Cmd* cmd, Param_Cursor* cursor, Cmd_Type type);

const Cmd CMD_LED = CMD_INIT(
  CMD_LED_NAME, 
  Cmd_Type_Any, 
  NULL, Led_onSet, Led_onGet, Led_onHelp, NULL
);

static Cmd_Handled Led_onSet(CmdManager* manager, Cmd* cmd, Param_Cursor* cursor, Cmd_Type type) {
  static const char ERROR[] = "\r\nERROR\r\n";
  static const char OK[] = "\r\nOK\r\n";
  Param param;
  UARTStream* uartStream = CmdManager_getArgs(manager);
  uint8_t num;
  uint8_t state;
  
  if (Param_next(cursor, &param) == NULL || 
      param.Value.Type != Param_ValueType_Number || 
      param.Value.Number > 1 || 
      param.Value.Number < 0
  ) {
    OStream_writeBytes(&uartStream->Output, (uint8_t*) ERROR, sizeof(ERROR) - 1);
    OStream_flush(&uartStream->Output);
    return Cmd_Done;
  }
  num = param.Value.Number;
  
  if (Param_next(cursor, &param) == NULL || 
    (param.Value.Type != Param_ValueType_Number && 
    param.Value.Type != Param_ValueType_Boolean && 
    param.Value.Type != Param_ValueType_State &&
    param.Value.Type != Param_ValueType_StateKey) ||
    param.Value.State > 1
  ) {
    OStream_writeBytes(&uartStream->Output, (uint8_t*) ERROR, sizeof(ERROR) - 1);
    OStream_flush(&uartStream->Output);
    return Cmd_Done;
  }
  state = param.Value.State;
  
  // classic way
  /*
  switch (num) {
    case 0:
      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, (GPIO_PinState) state);
      break;
    case 1:
      HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, (GPIO_PinState) state);
      break;
    default:
      
      break;
  }
  */
  // array way
  HAL_GPIO_WritePin(LEDS[num].GPIO, LEDS[num].Pin, (GPIO_PinState) state);
  
  OStream_writeBytes(&uartStream->Output, (uint8_t*) OK, sizeof(OK) - 1);
  OStream_flush(&uartStream->Output);
  return Cmd_Done;
}
static Cmd_Handled Led_onGet(CmdManager* manager, Cmd* cmd, Param_Cursor* cursor, Cmd_Type type) {
  static const char LED[] = "+LED: ";
  static const char ON[] = "On";
  static const char OFF[] = "Off";
  static const char OK[] = "\r\n\r\nOK\r\n";
  
  UARTStream* uartStream = CmdManager_getArgs(manager);
  OStream_writeBytes(&uartStream->Output, (uint8_t*) LED, sizeof(LED) - 1);
  OStream_writeBytes(&uartStream->Output, (uint8_t*) "0, ", 3);
  OStream_writeBytes(&uartStream->Output, 
  (uint8_t*) ((LED_GREEN_GPIO_Port->ODR & LED_GREEN_Pin) ? ON : OFF), 
  (LED_GREEN_GPIO_Port->ODR & LED_GREEN_Pin) ? sizeof(ON) - 1 : sizeof(OFF) - 1
  );
  
  OStream_writeBytes(&uartStream->Output, (uint8_t*) "\r\n", 2);
  
  OStream_writeBytes(&uartStream->Output, (uint8_t*) LED, sizeof(LED) - 1);
  OStream_writeBytes(&uartStream->Output, (uint8_t*) "1, ", 3);
  OStream_writeBytes(&uartStream->Output, 
  (uint8_t*) ((LED_RED_GPIO_Port->ODR & LED_RED_Pin) ? ON : OFF), 
  (LED_RED_GPIO_Port->ODR & LED_RED_Pin) ? sizeof(ON) - 1 : sizeof(OFF) - 1
  );
  
  OStream_writeBytes(&uartStream->Output, (uint8_t*) OK, sizeof(OK) - 1);
  
  OStream_flush(&uartStream->Output);
  return Cmd_Done;
}
static Cmd_Handled Led_onHelp(CmdManager* manager, Cmd* cmd, Param_Cursor* cursor, Cmd_Type type) {
  static const char HELP[] = "+LED: <NUM>, <State>\r\n\r\nOK\r\n";
  UARTStream* uartStream = CmdManager_getArgs(manager);
  OStream_writeBytes(&uartStream->Output, (uint8_t*) HELP, sizeof(HELP) - 1);
  OStream_flush(&uartStream->Output);
  return Cmd_Done;
}
