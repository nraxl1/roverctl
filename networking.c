#include <libpynq.h>

#define UART_RX_PIN = IO_AR0
#define UART_TX_PIN = IO_AR1

void initialize_networking(){
	switchbox_set_pin(UART_RX_PIN, SWB_UART0_RX);
	switchbox_set_pin(UART_TX_PIN, SWB_UART0_TX);
}

int outgoing_networking_handler(){
	while(1){
// if the send queue is full, sleep for 5ms
// if you end up sleeping 10 times in a row, notify systemd

}

int incoming_networking_handler(){
// check for length byte
// parse the rest accordingly
}
