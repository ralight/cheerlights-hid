#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_hid_led_colours(int r, int g, int b);

void on_connect(struct mosquitto *mosq, void *obj, int result)
{
	mosquitto_subscribe(mosq, NULL, "cheerlightsRGB", 0);
}


void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	int r, g, b;

	if(msg->payloadlen != 7) return;
	if(((char *)msg->payload)[0] != '#') return;

	if(sscanf((char *)msg->payload, "#%02x%02x%02x", &r, &g, &b) == 3){
		printf("Setting HID LED colour to #%02X%02X%02X\n", r, g, b);
		set_hid_led_colours(r, g, b);
	}
}


int main(int argc, char *argv[])
{
	struct mosquitto *mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new(NULL, 1, NULL);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	mosquitto_connect_bind(mosq, "mqtt.cheerlights.com", 1883, 60, NULL);
	mosquitto_loop_forever(mosq, -1, 1);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}
