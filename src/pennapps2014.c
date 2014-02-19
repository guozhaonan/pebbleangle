#include "pebble.h"

static Window *window;
static TextLayer *text_angle;

static int32_t angle;

static void send_angle() {
  Tuplet value = TupletInteger((uint8_t) 1, angle);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &value);
  dict_write_end(iter);

  app_message_outbox_send();
}

char text_buffer[40];

static void handle_accel(AccelData *accel_data, uint32_t num_samples) {
  angle = atan2_lookup(accel_data->z, accel_data->x) * 360 / TRIG_MAX_ANGLE - 270;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Angle %ld", angle);
  snprintf(text_buffer, sizeof(text_buffer), "%ld", angle);
  text_layer_set_text(text_angle, text_buffer);
}

static void window_load(Window *window) {
  text_angle = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_text(text_angle, "...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_angle));
}

static void window_unload(Window *window) {
  //nothing
}

//App Message Callbacks 

void timer_handler(void *context) {
  send_angle();
}

void send_delayed() {
  app_timer_register(100, timer_handler, NULL);
}

void outbox_failed(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox failed - %d", reason);
  send_delayed();
}

void outbox_sent(DictionaryIterator *iter, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox sent");
  send_delayed();
}

static void init() {
   window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  accel_data_service_subscribe(1, handle_accel);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);

  app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
  
  const int inbound_size = 64;
  const int outbound_size = 64;
  app_message_register_outbox_failed(outbox_failed);
  app_message_register_outbox_sent(outbox_sent);
  app_message_open(inbound_size, outbound_size);

  send_angle();
}

static void deinit() {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}