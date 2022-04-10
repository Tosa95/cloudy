#ifndef A5895F0A_A6FE_4C9B_AC88_9ABF0D91DCF6
#define A5895F0A_A6FE_4C9B_AC88_9ABF0D91DCF6

    #include <unity.h>
    #include "common.h"
    #include "../../lib/serial_comm/data_structures/pending_message_queue.h"
    #include "../../lib/time/types.h"

    using namespace SerialComm::DataStructures;
    using namespace SerialComm::Message;

    Time::millisecond_t currTime = 0;
    Time::millisecond_t getTime() {
        return currTime;
    }
    void test_pending_message_queue_add_and_retrieve(void) {
        PendingMessageQueue pmq(
            3,
            getTime,
            1000
        );

        TEST_ASSERT_EQUAL(0,pmq.size());
        TEST_ASSERT_TRUE(pmq.empty())
        TEST_ASSERT_FALSE(pmq.full());

        currTime = 10;

        StubCallback *sc1 = new StubCallback();
        StubDestructorData sdd;
        SerialComm::MessageManagers::Destructor *destructor = new StubDestructor(&sdd);
        Message msg1 = Message(
            Header{
                .source=1,
                .destination=42,
                .id=101,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            nullptr,
            destructor
        );

        TEST_ASSERT_EQUAL(1,pmq.add(msg1, sc1));
        TEST_ASSERT_EQUAL(1,pmq.size());
        TEST_ASSERT_FALSE(pmq.empty())
        TEST_ASSERT_FALSE(pmq.full());

        currTime = 20;

        StubCallback *sc2 = new StubCallback();
        StubDestructorData sdd2;
        SerialComm::MessageManagers::Destructor *destructor2 = new StubDestructor(&sdd2);
        Message msg2 = Message(
            Header{
                .source=2,
                .destination=42,
                .id=102,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            nullptr,
            destructor2
        );

        TEST_ASSERT_EQUAL(1, pmq.add(msg2, sc2));
        TEST_ASSERT_EQUAL(2,pmq.size());
        TEST_ASSERT_FALSE(pmq.empty())
        TEST_ASSERT_FALSE(pmq.full());

        currTime = 30;

        StubCallback *sc3 = new StubCallback();
        StubDestructorData sdd3;
        SerialComm::MessageManagers::Destructor *destructor3 = new StubDestructor(&sdd3);
        Message msg3 = Message(
            Header{
                .source=3,
                .destination=42,
                .id=103,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            nullptr,
            destructor3
        );

        TEST_ASSERT_EQUAL(1, pmq.add(msg3, sc3));
        TEST_ASSERT_EQUAL(3,pmq.size());
        TEST_ASSERT_FALSE(pmq.empty())
        TEST_ASSERT_TRUE(pmq.full());

        currTime = 40;

        StubCallback *sc4 = new StubCallback();
        StubDestructorData sdd4;
        SerialComm::MessageManagers::Destructor *destructor4 = new StubDestructor(&sdd4);
        Message msg4 = Message(
            Header{
                .source=4,
                .destination=42,
                .id=104,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            nullptr,
            destructor4
        );

        TEST_ASSERT_EQUAL(0, pmq.add(msg4, sc4));

        PendingMessageQueueEntry *got = pmq.get(102);

        TEST_ASSERT_TRUE(msg2 == got->msg);
        TEST_ASSERT_EQUAL(sc2, got->callback);
        TEST_ASSERT_EQUAL(20, got->received);

        TEST_ASSERT_EQUAL(nullptr, pmq.get(1234));

    }

    void test_pending_message_queue_add_and_delete_given_id(void) {
        PendingMessageQueue pmq(
            3,
            getTime,
            1000
        );

        currTime = 10;

        StubCallback *sc1 = new StubCallback();
        StubDestructorData sdd;
        StubDestructor *destructor = new StubDestructor(&sdd);
        Message msg1 = Message(
            Header{
                .source=1,
                .destination=42,
                .id=101,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            (void*)12344,
            destructor
        );

        pmq.add(msg1, sc1);

        currTime = 20;

        StubCallback *sc2 = new StubCallback();
        StubDestructorData sdd2;
        StubDestructor *destructor2 = new StubDestructor(&sdd2);
        Message msg2 = Message(
            Header{
                .source=2,
                .destination=42,
                .id=102,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            (void*)12345,
            destructor2
        );

        pmq.add(msg2, sc2);

        currTime = 30;

        StubCallback *sc3 = new StubCallback();
        StubDestructorData sdd3;
        StubDestructor *destructor3 = new StubDestructor(&sdd3);
        Message msg3 = Message(
            Header{
                .source=3,
                .destination=42,
                .id=103,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            (void*)12346,
            destructor3
        );

        pmq.add(msg3, sc3);

        currTime = 40;

        PendingMessageQueueEntry *got = pmq.get(102);
        void *expectedDestroyedData = got->msg.getData();

        TEST_ASSERT_EQUAL(102, got->msg.getHeader().id);
        TEST_ASSERT_EQUAL(2, got->msg.getHeader().source);
        TEST_ASSERT_EQUAL(42, got->msg.getHeader().destination);
        TEST_ASSERT_EQUAL(67, got->msg.getHeader().type);
        TEST_ASSERT_EQUAL(MessageDirection::REQUEST, got->msg.getHeader().direction);
        TEST_ASSERT_EQUAL((void*)12345, got->msg.getData());


        TEST_ASSERT_EQUAL(sc2, got->callback);
        TEST_ASSERT_EQUAL(20, got->received);

        TEST_ASSERT_EQUAL(nullptr, pmq.get(1234));

        TEST_ASSERT_EQUAL(1, pmq.remove(102));

        TEST_ASSERT_EQUAL(nullptr, pmq.get(102));
        TEST_ASSERT_EQUAL(1, sdd2.nCalls);
        TEST_ASSERT_EQUAL(expectedDestroyedData, sdd2.lastDestroyed);
        TEST_ASSERT_EQUAL(2, pmq.size());
        TEST_ASSERT_FALSE(pmq.full());
        TEST_ASSERT_FALSE(pmq.empty());

        TEST_ASSERT_EQUAL(1, pmq.remove(103));

        TEST_ASSERT_EQUAL(nullptr, pmq.get(103));
        TEST_ASSERT_EQUAL(1, sdd3.nCalls);
        TEST_ASSERT_EQUAL((void*)12346, sdd3.lastDestroyed);
        TEST_ASSERT_EQUAL(1, pmq.size());
        TEST_ASSERT_FALSE(pmq.full());
        TEST_ASSERT_FALSE(pmq.empty());

        TEST_ASSERT_EQUAL(1, pmq.remove(101));

        TEST_ASSERT_EQUAL(nullptr, pmq.get(101));
        TEST_ASSERT_EQUAL(1, sdd.nCalls);
        TEST_ASSERT_EQUAL((void*)12344, sdd.lastDestroyed);
        TEST_ASSERT_EQUAL(0, pmq.size());
        TEST_ASSERT_FALSE(pmq.full());
        TEST_ASSERT_TRUE(pmq.empty());
    }

    void test_pending_message_queue_add_and_delete_given_id_memory_leak(void) {

        PendingMessageQueue pmq(
            3,
            getTime,
            1000
        );

        for (int i = 0; i < N; i++) {

            #ifndef ARDUINO_ARCH
                yield();
            #endif

            currTime = 10;

            StubCallback *sc1 = new StubCallback();
            StubDestructorData sdd;
            StubDestructor *destructor = new StubDestructor(&sdd);
            Message msg1 = Message(
                Header{
                    .source=1,
                    .destination=42,
                    .id=101,
                    .type=67,
                    .direction=MessageDirection::REQUEST
                },
                (void*)12344,
                destructor
            );

            pmq.add(msg1, sc1);

            currTime = 20;

            StubCallback *sc2 = new StubCallback();
            StubDestructorData sdd2;
            StubDestructor *destructor2 = new StubDestructor(&sdd2);
            Message msg2 = Message(
                Header{
                    .source=2,
                    .destination=42,
                    .id=102,
                    .type=67,
                    .direction=MessageDirection::REQUEST
                },
                (void*)12345,
                destructor2
            );

            pmq.add(msg2, sc2);

            currTime = 30;

            StubCallback *sc3 = new StubCallback();
            StubDestructorData sdd3;
            StubDestructor *destructor3 = new StubDestructor(&sdd3);
            Message msg3 = Message(
                Header{
                    .source=3,
                    .destination=42,
                    .id=103,
                    .type=67,
                    .direction=MessageDirection::REQUEST
                },
                (void*)12346,
                destructor3
            );

            pmq.add(msg3, sc3);

            currTime = 40;

            PendingMessageQueueEntry *got = pmq.get(102);
            void *expectedDestroyedData = got->msg.getData();

            TEST_ASSERT_TRUE(msg2 == got->msg);
            TEST_ASSERT_EQUAL(sc2, got->callback);
            TEST_ASSERT_EQUAL(20, got->received);

            TEST_ASSERT_EQUAL(nullptr, pmq.get(1234));

            TEST_ASSERT_EQUAL(1, pmq.remove(102));

            TEST_ASSERT_EQUAL(nullptr, pmq.get(102));
            TEST_ASSERT_EQUAL(1, sdd2.nCalls);
            TEST_ASSERT_EQUAL(expectedDestroyedData, sdd2.lastDestroyed);
            TEST_ASSERT_EQUAL(2, pmq.size());
            TEST_ASSERT_FALSE(pmq.full());
            TEST_ASSERT_FALSE(pmq.empty());

            TEST_ASSERT_EQUAL(1, pmq.remove(103));

            TEST_ASSERT_EQUAL(nullptr, pmq.get(103));
            TEST_ASSERT_EQUAL(1, sdd3.nCalls);
            TEST_ASSERT_EQUAL((void*)12346, sdd3.lastDestroyed);
            TEST_ASSERT_EQUAL(1, pmq.size());
            TEST_ASSERT_FALSE(pmq.full());
            TEST_ASSERT_FALSE(pmq.empty());

            TEST_ASSERT_EQUAL(1, pmq.remove(101));

            TEST_ASSERT_EQUAL(nullptr, pmq.get(101));
            TEST_ASSERT_EQUAL(1, sdd.nCalls);
            TEST_ASSERT_EQUAL((void*)12344, sdd.lastDestroyed);
            TEST_ASSERT_EQUAL(0, pmq.size());
            TEST_ASSERT_FALSE(pmq.full());
            TEST_ASSERT_TRUE(pmq.empty());

            delete got;
        }

    }

    void test_pending_message_queue_add_and_delete_for_timeout(void) {
        PendingMessageQueue pmq(
            3,
            getTime,
            100
        );

        currTime = 10;

        StubCallback *sc1 = new StubCallback();
        StubDestructorData sdd;
        StubDestructor *destructor = new StubDestructor(&sdd);
        Message msg1 = Message(
            Header{
                .source=1,
                .destination=42,
                .id=101,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            (void*)12344,
            destructor
        );

        pmq.add(msg1, sc1);

        currTime = 20;

        StubCallback *sc2 = new StubCallback();
        StubDestructorData sdd2;
        StubDestructor *destructor2 = new StubDestructor(&sdd2);
        Message msg2 = Message(
            Header{
                .source=2,
                .destination=42,
                .id=102,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            (void*)12345,
            destructor2
        );

        pmq.add(msg2, sc2);

        currTime = 30;

        StubCallback *sc3 = new StubCallback();
        StubDestructorData sdd3;
        StubDestructor *destructor3 = new StubDestructor(&sdd3);
        Message msg3 = Message(
            Header{
                .source=3,
                .destination=42,
                .id=103,
                .type=67,
                .direction=MessageDirection::REQUEST
            },
            (void*)12346,
            destructor3
        );

        pmq.add(msg3, sc3);

        currTime = 100;

        StubTimeoutMessageSender *tms = new StubTimeoutMessageSender();

        pmq.removeOldMessages(tms);
        TEST_ASSERT_EQUAL(3, pmq.size());
        TEST_ASSERT_EQUAL(0, tms->nCalls);

        currTime = 111;
        pmq.removeOldMessages(tms);
        TEST_ASSERT_EQUAL(2, pmq.size());
        TEST_ASSERT_EQUAL(1, tms->nCalls);
        TEST_ASSERT_TRUE(msg1 == tms->lastMessage);
        TEST_ASSERT_EQUAL(sc1, tms->lastCallback);

        currTime = 119;
        pmq.removeOldMessages(tms);
        TEST_ASSERT_EQUAL(2, pmq.size());
        TEST_ASSERT_EQUAL(1, tms->nCalls);

        currTime = 125;
        pmq.removeOldMessages(tms);
        TEST_ASSERT_EQUAL(1, pmq.size());
        TEST_ASSERT_EQUAL(2, tms->nCalls);
        TEST_ASSERT_TRUE(msg2 == tms->lastMessage);
        TEST_ASSERT_EQUAL(sc2, tms->lastCallback);

        currTime = 200;
        pmq.removeOldMessages(tms);
        TEST_ASSERT_EQUAL(0, pmq.size());
        TEST_ASSERT_EQUAL(3, tms->nCalls);
        TEST_ASSERT_TRUE(msg3 == tms->lastMessage);
        TEST_ASSERT_EQUAL(sc3, tms->lastCallback);
        TEST_ASSERT_TRUE(pmq.empty());

        delete tms;
    }

    void test_pending_message_queue_add_and_delete_for_timeout_memory_leak(void) {
        
        PendingMessageQueue pmq(
            3,
            getTime,
            100
        );

        for (int i = 0; i < N; i++) {
            #ifndef ARDUINO_ARCH
                yield();
            #endif
            currTime = 10;

            StubCallback *sc1 = new StubCallback();
            StubDestructorData sdd;
            StubDestructor *destructor = new StubDestructor(&sdd);
            Message msg1 = Message(
                Header{
                    .source=1,
                    .destination=42,
                    .id=101,
                    .type=67,
                    .direction=MessageDirection::REQUEST
                },
                (void*)12344,
                destructor
            );

            pmq.add(msg1, sc1);

            currTime = 20;

            StubCallback *sc2 = new StubCallback();
            StubDestructorData sdd2;
            StubDestructor *destructor2 = new StubDestructor(&sdd2);
            Message msg2 = Message(
                Header{
                    .source=2,
                    .destination=42,
                    .id=102,
                    .type=67,
                    .direction=MessageDirection::REQUEST
                },
                (void*)12345,
                destructor2
            );

            pmq.add(msg2, sc2);

            currTime = 30;

            StubCallback *sc3 = new StubCallback();
            StubDestructorData sdd3;
            StubDestructor *destructor3 = new StubDestructor(&sdd3);
            Message msg3 = Message(
                Header{
                    .source=3,
                    .destination=42,
                    .id=103,
                    .type=67,
                    .direction=MessageDirection::REQUEST
                },
                (void*)12346,
                destructor3
            );

            pmq.add(msg3, sc3);

            currTime = 100;

            StubTimeoutMessageSender *tms = new StubTimeoutMessageSender();

            pmq.removeOldMessages(tms);
            TEST_ASSERT_EQUAL(3, pmq.size());

            currTime = 111;
            pmq.removeOldMessages(tms);
            TEST_ASSERT_EQUAL(2, pmq.size());

            currTime = 119;
            pmq.removeOldMessages(tms);
            TEST_ASSERT_EQUAL(2, pmq.size());

            currTime = 135;
            pmq.removeOldMessages(tms);
            TEST_ASSERT_EQUAL(0, pmq.size());

            TEST_ASSERT_TRUE(pmq.empty());

            delete tms;
        }

    }    

#endif /* A5895F0A_A6FE_4C9B_AC88_9ABF0D91DCF6 */
