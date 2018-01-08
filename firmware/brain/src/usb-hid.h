#pragma once

void HID_update(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

#define DEBUG_USB_NOTREADY    (_BV(0))
#define DEBUG_USB_ENUMERATING (_BV(1) | _BV(2))
#define DEBUG_USB_READY       (_BV(1) | _BV(3))
#define DEBUG_USB_ERROR       (_BV(0) | _BV(3))
