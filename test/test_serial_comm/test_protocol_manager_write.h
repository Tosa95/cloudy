#ifndef E52A33A4_0918_4206_A649_7114786135B53
#define E52A33A4_0918_4206_A649_7114786135B53

    #include <unity.h>
    #include <stdint.h>

    #include "common.h"

    #include "../../lib/serial_comm/test_support/stub_stream.h"
    #include "../../lib/serial_comm/protocol_manager.h"
    #include "../../lib/serial_comm/responder.h"
    #include "../../lib/serial_comm/data_structures/message_type_map.h"
    #include "../../lib/serial_comm/message_managers/message_manager_factories.h"
    #include "../../lib/serial_comm/drivers/stream_inbound_data_driver.h"
    #include "../../lib/serial_comm/drivers/print_outbound_data_driver.h"
    #include "../../lib/serial_comm/message/std/unknown_message.h"
    #include "../../lib/time/types.h"
    #include "../../lib/serial_comm/message/message.h"
    #include "../../lib/serial_comm/message/callback.h"

    using namespace SerialComm::MessageManagers;
    using namespace SerialComm::Message;

    void test_protocol_manager_outbound_no_callback(void) {
        int outboundDataSize = 100;
        uint8_t outboundData[outboundDataSize];

        uint8_t expectedData[] = {
            // Message 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            64,0,18,4,0,0,0,4,SerialComm::Message::MessageDirection::REQUEST,
            123,1,0,0,2,
            // Message 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            64,0,18,4,1,0,0,4,SerialComm::Message::MessageDirection::REQUEST,
            136,1,0,0,2
        };
        size_t expectedDataSize = sizeof(expectedData)/sizeof(uint8_t);
        size_t msgSize = 19;

        SerialComm::TestSupport::StubStream stream(
            nullptr,
            0,
            outboundData,
            outboundDataSize
        );

        SerialComm::DataStructures::MessageTypeMap mtm;
        SerialComm::DataStructures::PendingMessageQueue pmq(
            10,
            []()->Time::millisecond_t{return 0;},
            100
        );
        SerialComm::ModuloNMessageIdManager midmng(42);

        StubCallback clbk;
        mtm.add(
            1024,
            SerialComm::MessageManagers::createDirectMemoryMessageManager(
                sizeof(Data)
            ),
            &clbk
        );

        SerialComm::Drivers::StreamInboundDataDriver id(&stream);
        SerialComm::Drivers::PrintOutboundDataDriver od(&stream);

        SerialComm::ProtocolManager pm(
            64,
            &mtm,
            &pmq,
            &id,
            &od,
            &midmng,
            (MessageManager *)&SerialComm::Message::PREAMBLE_CODER,
            (MessageManager *)&SerialComm::Message::MESSAGE_HEADER_CODER
        );

        for (int i = 0; i < N; i++) {

            stream.seekOutbound(0);

            Data *msgBody = new Data {
                .n1 = 123,
                .n2 = 33554433
            };

            pm.sendMessage(
                1042,
                1024,
                msgBody
            );

            for (int i = 0; i < msgSize; i++) {
                TEST_ASSERT_EQUAL(expectedData[i],outboundData[i]);
            }

            msgBody = new Data {
                .n1 = 136,
                .n2 = 33554433
            };

            pm.sendMessage(
                1042,
                1024,
                msgBody
            );

            for (int i = msgSize; i < 2*msgSize; i++) {
                TEST_ASSERT_EQUAL(expectedData[i],outboundData[i]);
            }

            midmng.reset();
            pm.update();

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

    void test_protocol_manager_outbound_with_callback(void) {
        int outboundDataSize = 100;
        uint8_t outboundData[outboundDataSize];

        uint8_t expectedData[] = {
            // Message 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            64,0,18,4,0,0,0,4,SerialComm::Message::MessageDirection::REQUEST,
            123,1,0,0,2,
            // Message 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            64,0,18,4,1,0,0,4,SerialComm::Message::MessageDirection::REQUEST,
            136,1,0,0,2
        };
        size_t expectedDataSize = sizeof(expectedData)/sizeof(uint8_t);
        size_t msgSize = 19;

        SerialComm::TestSupport::StubStream stream(
            nullptr,
            0,
            outboundData,
            outboundDataSize
        );

        SerialComm::DataStructures::MessageTypeMap mtm;
        SerialComm::DataStructures::PendingMessageQueue pmq(
            10,
            []()->Time::millisecond_t{return 0;},
            100
        );
        SerialComm::ModuloNMessageIdManager midmng(42);

        StubCallback clbk;
        mtm.add(
            1024,
            SerialComm::MessageManagers::createDirectMemoryMessageManager(
                sizeof(Data)
            ),
            &clbk
        );

        SerialComm::Drivers::StreamInboundDataDriver id(&stream);
        SerialComm::Drivers::PrintOutboundDataDriver od(&stream);

        SerialComm::ProtocolManager pm(
            64,
            &mtm,
            &pmq,
            &id,
            &od,
            &midmng,
            (MessageManager *)&SerialComm::Message::PREAMBLE_CODER,
            (MessageManager *)&SerialComm::Message::MESSAGE_HEADER_CODER
        );

        for (int i = 0; i < N; i++) {
            stream.seekOutbound(0);

            Data *msgBody = new Data {
                .n1 = 123,
                .n2 = 33554433
            };

            MessageCallback *clbk1 = new StubCallback();
            pm.sendMessage(
                1042,
                1024,
                msgBody,
                clbk1
            );

            for (int i = 0; i < msgSize; i++) {
                TEST_ASSERT_EQUAL(expectedData[i],outboundData[i]);
            }

            msgBody = new Data {
                .n1 = 136,
                .n2 = 33554433
            };

            MessageCallback *clbk2 = new StubCallback();
            pm.sendMessage(
                1042,
                1024,
                msgBody,
                clbk2
            );

            for (int i = msgSize; i < 2*msgSize; i++) {
                TEST_ASSERT_EQUAL(expectedData[i],outboundData[i]);
            }

            TEST_ASSERT_EQUAL(2, pmq.size());

            SerialComm::DataStructures::PendingMessageQueueEntry *e = pmq.get(0);
            TEST_ASSERT_NOT_EQUAL(nullptr, e);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n1, 123);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n2, 33554433);
            TEST_ASSERT_EQUAL(e->callback, clbk1);
            delete e;

            e = pmq.get(1);
            TEST_ASSERT_NOT_EQUAL(nullptr, e);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n1, 136);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n2, 33554433);
            TEST_ASSERT_EQUAL(e->callback, clbk2);
            delete e;

            pmq.clear();
            midmng.reset();
            pm.update();

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

    Time::millisecond_t currTimePMQ = 0;
    void test_protocol_manager_outbound_with_callback_timeout_messages_are_sent(void) {
        int outboundDataSize = 100;
        uint8_t outboundData[outboundDataSize];

        SerialComm::TestSupport::StubStream stream(
            nullptr,
            0,
            outboundData,
            outboundDataSize
        );

        SerialComm::DataStructures::MessageTypeMap mtm;
        SerialComm::DataStructures::PendingMessageQueue pmq(
            10,
            []()->Time::millisecond_t{return currTimePMQ;},
            100
        );
        SerialComm::ModuloNMessageIdManager midmng(42);

        StubCallback clbk;
        mtm.add(
            1024,
            SerialComm::MessageManagers::createDirectMemoryMessageManager(
                sizeof(Data)
            ),
            &clbk
        );

        SerialComm::Drivers::StreamInboundDataDriver id(&stream);
        SerialComm::Drivers::PrintOutboundDataDriver od(&stream);

        SerialComm::ProtocolManager pm(
            64,
            &mtm,
            &pmq,
            &id,
            &od,
            &midmng,
            (MessageManager *)&SerialComm::Message::PREAMBLE_CODER,
            (MessageManager *)&SerialComm::Message::MESSAGE_HEADER_CODER
        );

        for (int i = 0; i < N; i++) {
            stream.seekOutbound(0);

            Data *msgBody = new Data {
                .n1 = 123,
                .n2 = 33554433
            };

            StubCallbackData clbk1Data;
            StubCallback *clbk1 = new StubCallback(&clbk1Data);
            pm.sendMessage(
                1042,
                1024,
                msgBody,
                clbk1
            );

            msgBody = new Data {
                .n1 = 136,
                .n2 = 33554433
            };

            StubCallbackData clbk2Data;
            StubCallback *clbk2 = new StubCallback(&clbk2Data);
            pm.sendMessage(
                1042,
                1024,
                msgBody,
                clbk2
            );

            TEST_ASSERT_EQUAL(2, pmq.size());

            SerialComm::DataStructures::PendingMessageQueueEntry *e = pmq.get(0);
            TEST_ASSERT_NOT_EQUAL(nullptr, e);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n1, 123);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n2, 33554433);
            TEST_ASSERT_EQUAL(e->callback, clbk1);
            delete e;

            e = pmq.get(1);
            TEST_ASSERT_NOT_EQUAL(nullptr, e);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n1, 136);
            TEST_ASSERT_EQUAL((*(Data *)(e->msg.getData())).n2, 33554433);
            TEST_ASSERT_EQUAL(e->callback, clbk2);
            delete e;

            pm.update();
            TEST_ASSERT_EQUAL(2, pmq.size());
            TEST_ASSERT_EQUAL(0, clbk1Data.callCount);
            TEST_ASSERT_EQUAL(0, clbk2Data.callCount);

            currTimePMQ += 120;

            pm.update();
            TEST_ASSERT_EQUAL(0, pmq.size());
            TEST_ASSERT_EQUAL(1, clbk1Data.callCount);
            TEST_ASSERT_EQUAL(TIMEOUT_MESSAGE_TYPE, clbk1Data.lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(1042, clbk1Data.lastReceivedHeader.source);
            TEST_ASSERT_EQUAL(64, clbk1Data.lastReceivedHeader.destination);
            TEST_ASSERT_EQUAL(0, clbk1Data.lastReceivedHeader.id);
            TEST_ASSERT_EQUAL(MessageDirection::RESPONSE, clbk1Data.lastReceivedHeader.direction);

            TEST_ASSERT_EQUAL(1, clbk2Data.callCount);
            TEST_ASSERT_EQUAL(TIMEOUT_MESSAGE_TYPE, clbk2Data.lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(TIMEOUT_MESSAGE_TYPE, clbk2Data.lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(1042, clbk2Data.lastReceivedHeader.source);
            TEST_ASSERT_EQUAL(64, clbk2Data.lastReceivedHeader.destination);
            TEST_ASSERT_EQUAL(1, clbk2Data.lastReceivedHeader.id);
            TEST_ASSERT_EQUAL(MessageDirection::RESPONSE, clbk2Data.lastReceivedHeader.direction);

            midmng.reset();
            pm.update();

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

    void test_protocol_manager_outbound_response(void) {
        int outboundDataSize = 100;
        uint8_t outboundData[outboundDataSize];

        uint8_t expectedData[] = {
            // Message 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            64,0,116,2,13,0,0,4,SerialComm::Message::MessageDirection::RESPONSE,
            123,1,0,0,2,
            // Message 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            64,0,116,2,123,0,0,4,SerialComm::Message::MessageDirection::RESPONSE,
            136,1,0,0,2
        };
        size_t expectedDataSize = sizeof(expectedData)/sizeof(uint8_t);
        size_t msgSize = 19;

        SerialComm::TestSupport::StubStream stream(
            nullptr,
            0,
            outboundData,
            outboundDataSize
        );

        SerialComm::DataStructures::MessageTypeMap mtm;
        SerialComm::DataStructures::PendingMessageQueue pmq(
            10,
            []()->Time::millisecond_t{return 0;},
            100
        );
        SerialComm::ModuloNMessageIdManager midmng(42);

        StubCallback clbk;
        mtm.add(
            1024,
            SerialComm::MessageManagers::createDirectMemoryMessageManager(
                sizeof(Data)
            ),
            &clbk
        );

        SerialComm::Drivers::StreamInboundDataDriver id(&stream);
        SerialComm::Drivers::PrintOutboundDataDriver od(&stream);

        SerialComm::ProtocolManager pm(
            64,
            &mtm,
            &pmq,
            &id,
            &od,
            &midmng,
            (MessageManager *)&SerialComm::Message::PREAMBLE_CODER,
            (MessageManager *)&SerialComm::Message::MESSAGE_HEADER_CODER
        );

        for (int i = 0; i < N; i++) {
            stream.seekOutbound(0);

            Message originalMessage1(
                Header {
                    .source = 628,
                    .destination = 64,
                    .id = 13,
                    .type = 1024,
                    .direction = MessageDirection::REQUEST
                },
                new Data {12, 33554433},
                new DirectMemoryDestructor()
            );

            Data *responseBody1 = new Data {
                .n1 = 123,
                .n2 = 33554433
            };

            pm.sendResponse(
                originalMessage1,
                1024,
                responseBody1
            );

            originalMessage1.destroy();

            for (int i = 0; i < msgSize; i++) {
                TEST_ASSERT_EQUAL(expectedData[i],outboundData[i]);
            }

            Message originalMessage2(
                Header {
                    .source = 628,
                    .destination = 64,
                    .id = 123,
                    .type = 1024,
                    .direction = MessageDirection::REQUEST
                },
                new Data {12, 33554433},
                new DirectMemoryDestructor()
            );

            Data *responseBody2 = new Data {
                .n1 = 136,
                .n2 = 33554433
            };

            pm.sendResponse(
                originalMessage2,
                1024,
                responseBody2
            );

            originalMessage2.destroy();

            for (int i = msgSize; i < 2*msgSize; i++) {
                TEST_ASSERT_EQUAL(expectedData[i],outboundData[i]);
            }

            midmng.reset();
            pm.update();

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

    static Time::millisecond_t currTimeOSS;
    void test_protocol_manager_outbound_send_sync(void) {
        // Serial.println(freeMemory());
        uint8_t inboundData[] = {
            // Noise
            13,178,
            // Response 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            116,2,64,0,0,0,0,4,SerialComm::Message::MessageDirection::RESPONSE,
            13,1,0,0,0,
            // Noise
            0b10101010,0b10101010,23,55,0b10101010,0b10101010,0b10101010,0b10101010,11,
            // Response 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            116,0,64,0,1,0,0,4,SerialComm::Message::MessageDirection::RESPONSE,
            17,42,0,0,0
        };
        size_t inboundSize = sizeof(inboundData)/sizeof(uint8_t);

        SerialComm::TestSupport::StubStream stream(
            inboundData,
            inboundSize,
            nullptr,
            0
        );

        SerialComm::DataStructures::MessageTypeMap mtm;
        SerialComm::DataStructures::PendingMessageQueue pmq(
            10,
            []()->Time::millisecond_t{
                currTimeOSS += 10;
                return currTimeOSS;
            },
            100
        );
        SerialComm::ModuloNMessageIdManager midmng(42);

        StubCallback clbk;
        mtm.add(
            1024,
            SerialComm::MessageManagers::createDirectMemoryMessageManager(
                sizeof(Data)
            ),
            &clbk
        );

        SerialComm::Drivers::StreamInboundDataDriver id(&stream);
        SerialComm::Drivers::PrintOutboundDataDriver od(&stream);

        SerialComm::ProtocolManager pm(
            64,
            &mtm,
            &pmq,
            &id,
            &od,
            &midmng,
            (MessageManager *)&SerialComm::Message::PREAMBLE_CODER,
            (MessageManager *)&SerialComm::Message::MESSAGE_HEADER_CODER
        );


        for (int i = 0; i < N; i++) {

            stream.setReadUntil(23);

            Message response = pm.sendMessageSync(
                628,
                1024,
                new Data {12, 17}
            );

            TEST_ASSERT_EQUAL(628, response.getHeader().source);
            TEST_ASSERT_EQUAL(64, response.getHeader().destination);
            TEST_ASSERT_EQUAL(0, response.getHeader().id);
            TEST_ASSERT_EQUAL(1024, response.getHeader().type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::RESPONSE, response.getHeader().direction);

            TEST_ASSERT_EQUAL(13, ((Data *)response.getData())->n1);
            TEST_ASSERT_EQUAL(1, ((Data *)response.getData())->n2);

            response.destroy();

            stream.setReadUntil(inboundSize);

            response = pm.sendMessageSync(
                116,
                1024,
                new Data {13, 1789}
            );

            TEST_ASSERT_EQUAL(116, response.getHeader().source);
            TEST_ASSERT_EQUAL(64, response.getHeader().destination);
            TEST_ASSERT_EQUAL(1, response.getHeader().id);
            TEST_ASSERT_EQUAL(1024, response.getHeader().type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::RESPONSE, response.getHeader().direction);

            TEST_ASSERT_EQUAL(17, ((Data *)response.getData())->n1);
            TEST_ASSERT_EQUAL(42, ((Data *)response.getData())->n2);

            response.destroy();

            response = pm.sendMessageSync(
                113,
                1024,
                new Data {16, 17}
            );

            TEST_ASSERT_EQUAL(113, response.getHeader().source);
            TEST_ASSERT_EQUAL(64, response.getHeader().destination);
            TEST_ASSERT_EQUAL(2, response.getHeader().id);
            TEST_ASSERT_EQUAL(TIMEOUT_MESSAGE_TYPE, response.getHeader().type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::RESPONSE, response.getHeader().direction);

            response.destroy();


            stream.seekInbound(0);
            midmng.reset();

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

#endif /* E52A33A4_0918_4206_A649_7114786135B53 */
