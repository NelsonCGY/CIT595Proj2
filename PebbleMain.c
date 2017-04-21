#include <pebble.h>

static Window *window, *notification, *setTemp;
static TextLayer *hello_layer, *notification_layer, *setTemp_layer;
static char msg[100];
static char str[100];
static bool standby = false;
static int threshold = 30;

int exceed_threshold() {
  if (strlen(msg) <= 6) return -1;
	int now_int = 0;
	char now_char[8] = {0};
	if (msg[0] == 'N' && msg[1] == 'o' && msg[2] == 'w' && msg[3] == ':' && msg[4] == ' '){
    if (msg[5] == 'S') return -1;
		now_int = atoi(strncpy(now_char, msg + 5, 2));
	}
	else return -1;

	unsigned int j = 0;
	for (unsigned int i = 0; i < strlen(msg); i++) {
		if (msg[i] == 'U') {
			j = i;
			break;
		}
	}

	if (j == 0) return -1;
	if (j + 6 > strlen(msg)) return -1;

	char unit_char;
	if (msg[j] == 'U' && msg[j + 1] == 'n' && msg[j + 2] == 'i' && msg[j + 3] == 't' && msg[j + 4] == ':' && msg[j + 5] == ' ') {
		unit_char = msg[j + 6];
	}
	else return -1;

	if (unit_char == 'F') {
		now_int = (now_int - 32) * 5 / 9;
	}

	if (now_int >= threshold) return 1;
	else return 0;
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered -- do nothing
}
void out_failed_handler(DictionaryIterator *failed,
  AppMessageResult reason, void *context) {
  // outgoing message failed
  text_layer_set_text(hello_layer, "Disconnected from phone!");
}
void in_received_handler(DictionaryIterator *received, void *context)
{
  // incoming message received
  // looks for key #0 in the incoming message
  int key = 0;
  Tuple *text_tuple = dict_find(received, key);
  if (text_tuple) {
    if (text_tuple->value) {
      // put it in this global variable
      strcpy(msg, text_tuple->value->cstring);
    }
    else strcpy(msg, "no value!");
    text_layer_set_text(hello_layer, msg);
    if(exceed_threshold() == 1){
      window_stack_push(notification, true);
    }
  }else {
    text_layer_set_text(hello_layer, "no message!");
  }
}
void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
  text_layer_set_text(hello_layer, "Error in!");
}
/* This is called when the select button is clicked */
void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
  if(!standby){
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    int key = 0;
    // send the message "hello?" to the phone, using key #0
    Tuplet value = TupletCString(key, "Temperature");
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }
}
/* This is called when the up button is clicked */
void long_click_up_handler(ClickRecognizerRef recognizer, void *context)
{
  if(!standby){
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    int key = 1;
    // send the message "hello?" to the phone, using key #0
    Tuplet value = TupletCString(key, "Switch");
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }
}

void long_click_down_handler(ClickRecognizerRef recognizer, void *context){
  //Do nothing
}

void time_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if(!standby){
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    int key = 2;
    Tuplet value = TupletCString(key, "Time");
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }
}

void stand_by_handler(ClickRecognizerRef recognizer, void *context){
  if(standby){
    text_layer_set_text(hello_layer, "Resume!");
    standby = false;
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    int key = 3;
    Tuplet value = TupletCString(key, "standby");
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }else{
    text_layer_set_text(hello_layer, "Standby");
    standby = true;
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    int key = 3;
    Tuplet value = TupletCString(key, "standby");
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }
}

void long_up_up_handler(ClickRecognizerRef recognizer, void *context)
{
  if(!standby){
    window_stack_push(setTemp, true);
  }
}

void setTemp_up_handler(ClickRecognizerRef recognizer, void *context){
  threshold++;
  snprintf(str, 99, "Current threshold: %d C", threshold);
  text_layer_set_text(setTemp_layer, str);
}

void setTemp_down_handler(ClickRecognizerRef recognizer, void *context){
  threshold--;
  snprintf(str, 99, "Current threshold: %d C", threshold);
  text_layer_set_text(setTemp_layer, str);
}

/* this registers the appropriate function to the appropriate button */
void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, time_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 1000, long_click_up_handler, long_click_down_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, stand_by_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 1000, long_up_up_handler, long_click_down_handler);
}

void setTemp_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, setTemp_down_handler);
  window_single_click_subscribe(BUTTON_ID_UP, setTemp_up_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  hello_layer = text_layer_create((GRect)
  { .origin = { 0, 30 },
    .size = { bounds.size.w, bounds.size.h } });
    text_layer_set_text(hello_layer, "Temperature Reader");
    text_layer_set_text_alignment(hello_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(hello_layer, GTextOverflowModeWordWrap);
    layer_add_child(window_layer, text_layer_get_layer(hello_layer));
}
static void window_unload(Window *window) {
  text_layer_destroy(hello_layer);
}
static void notification_load(Window *window) {
  Layer *window_layer = window_get_root_layer(notification);
  GRect bounds = layer_get_bounds(window_layer);
  notification_layer = text_layer_create((GRect)
  { .origin = { 0, 30 },
    .size = { bounds.size.w, bounds.size.h } });
    text_layer_set_text(notification_layer, "Temp limit excessed!");
    text_layer_set_text_alignment(notification_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(notification_layer, GTextOverflowModeWordWrap);
    layer_add_child(window_layer, text_layer_get_layer(notification_layer));
}
static void notification_unload(Window *window) {
  text_layer_destroy(notification_layer);
}

static void setTemp_load(Window *window) {
  Layer *window_layer = window_get_root_layer(setTemp);
  GRect bounds = layer_get_bounds(window_layer);
  setTemp_layer = text_layer_create((GRect)
  { .origin = { 0, 30 },
    .size = { bounds.size.w, bounds.size.h } });
    snprintf(str, 99, "Current threshold: %d C", threshold);
    text_layer_set_text(setTemp_layer, str);
    text_layer_set_text_alignment(setTemp_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(setTemp_layer, GTextOverflowModeWordWrap);
    layer_add_child(window_layer, text_layer_get_layer(setTemp_layer));
}
static void setTemp_unload(Window *window) {
  text_layer_destroy(setTemp_layer);
}

static void init(void) {
  window = window_create();
  notification = window_create();
  setTemp = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_set_window_handlers(notification, (WindowHandlers) {
    .load = notification_load,
    .unload = notification_unload,
  });
  window_set_window_handlers(setTemp, (WindowHandlers) {
    .load = setTemp_load,
    .unload = setTemp_unload,
  });
  // need this for adding the listener
  window_set_click_config_provider(window, config_provider);
  window_set_click_config_provider(setTemp, setTemp_config_provider);
  // for registering AppMessage handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  const bool animated = true;
  window_stack_push(window, animated);
}
static void deinit(void) {
  window_destroy(window);
  window_destroy(notification);
  window_destroy(setTemp);
}
int main(void) {
  init();
  app_event_loop();
  deinit();
}
