#include <Arduino.h>
#include <unity.h>
#include <stdint.h>

#include "common.h"
#include "test_direct_memory_coding.h"
#include "test_protocol_manager_read.h"
#include "test_protocol_manager_write.h"
#include "test_pending_message_queue.h"
#include "test_message_id_managers.h"

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(3000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_direct_memory_serialize);
    RUN_TEST(test_direct_memory_deserialize);
    RUN_TEST(test_direct_memory_deserialize_heap);
    RUN_TEST(test_size_agnostic_direct_memory_serialize);
    RUN_TEST(test_size_agnostic_direct_memory_deserialize);
    RUN_TEST(test_size_agnostic_direct_memory_deserialize_heap);

    // RUN_TEST(test_protocol_manager_inbound);
    // RUN_TEST(test_protocol_manager_inbound_unknown_message_type);
    // RUN_TEST(test_protocol_manager_inbound_unknown_message_type_no_callback);
    // RUN_TEST(test_protocol_manager_inbound_response);
    // RUN_TEST(test_protocol_manager_outbound_no_callback);
    // RUN_TEST(test_protocol_manager_outbound_with_callback);
    // RUN_TEST(test_protocol_manager_outbound_with_callback_timeout_messages_are_sent);
    // RUN_TEST(test_protocol_manager_outbound_response);
    // RUN_TEST(test_protocol_manager_outbound_send_sync);
    
    // RUN_TEST(test_modulo_n_message_id_manager);

    // RUN_TEST(test_pending_message_queue_add_and_retrieve);
    // RUN_TEST(test_pending_message_queue_add_and_delete_given_id);
    // RUN_TEST(test_pending_message_queue_add_and_delete_given_id_memory_leak);
    // RUN_TEST(test_pending_message_queue_add_and_delete_for_timeout);
    // RUN_TEST(test_pending_message_queue_add_and_delete_for_timeout_memory_leak);

    UNITY_END();
}

void loop() {

}