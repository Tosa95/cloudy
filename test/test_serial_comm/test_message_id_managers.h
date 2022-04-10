#ifndef BC3DE4B2_99C5_4928_B992_9C9A6EDC42D9
#define BC3DE4B2_99C5_4928_B992_9C9A6EDC42D9

    #include <unity.h>
    #include "../../lib/serial_comm/utils/message_id_managers.h"

    void test_modulo_n_message_id_manager(void) {
        SerialComm::ModuloNMessageIdManager manager(5);
        TEST_ASSERT_EQUAL(0, manager.get());

        manager.inc();
        TEST_ASSERT_EQUAL(1, manager.get());

        manager.inc();
        manager.inc();
        manager.inc();
        TEST_ASSERT_EQUAL(4, manager.get());

        manager.inc();
        TEST_ASSERT_EQUAL(0, manager.get());
    }

#endif /* BC3DE4B2_99C5_4928_B992_9C9A6EDC42D9 */
