#ifndef E52A33A4_0918_4206_A649_7114786135B5
#define E52A33A4_0918_4206_A649_7114786135B5

    #include <unity.h>
    #include <stdint.h>

    #include "common.h"
    #include <stdlib.h>
    #include "../../lib/memory_utils/free_memory.h"

    #include "../../lib/serial_comm/test_support/stub_stream.h"
    #include "../../lib/serial_comm/protocol_manager.h"
    #include "../../lib/serial_comm/responder.h"
    #include "../../lib/serial_comm/data_structures/message_type_map.h"
    #include "../../lib/serial_comm/message_managers/message_manager_factories.h"
    #include "../../lib/serial_comm/drivers/stream_inbound_data_driver.h"
    #include "../../lib/serial_comm/drivers/print_outbound_data_driver.h"
    #include "../../lib/serial_comm/message/std/unknown_message.h"
    #include "../../lib/time/types.h"

    using namespace SerialComm::MessageManagers;
    using namespace SerialComm::Message;

    void test_protocol_manager_inbound(void) {

        uint8_t inboundData[] = {
            // Noise
            13,178,
            // Message 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            115,0,116,2,5,1,0,4,SerialComm::Message::MessageDirection::REQUEST,
            12,1,0,0,2,
            // Noise
            0b10101010,0b10101010,23,55,0b10101010,0b10101010,0b10101010,0b10101010,11,
            // Message 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            123,0,116,0,5,0,0,4,SerialComm::Message::MessageDirection::REQUEST,
            12,15,0,0,0
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
            []()->Time::millisecond_t{return 0;},
            100
        );
        SerialComm::ModuloNMessageIdManager midmng(42);

        StubCallback clbk;
        MessageManager mm = SerialComm::MessageManagers::createDirectMemoryMessageManager(
            sizeof(Data)
        );
        mtm.add(
            1024,
            mm,
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
            stream.setReadUntil(12);
            pm.update();

            TEST_ASSERT_EQUAL(i*2+0, clbk.stubClbkData->callCount);

            stream.setReadUntil(25);
            pm.update();

            TEST_ASSERT_EQUAL(i*2+1, clbk.stubClbkData->callCount);
            TEST_ASSERT_EQUAL(115, clbk.stubClbkData->lastReceivedHeader.source);
            TEST_ASSERT_EQUAL(628, clbk.stubClbkData->lastReceivedHeader.destination);
            TEST_ASSERT_EQUAL(261, clbk.stubClbkData->lastReceivedHeader.id);
            TEST_ASSERT_EQUAL(1024, clbk.stubClbkData->lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::REQUEST, clbk.stubClbkData->lastReceivedHeader.direction);

            TEST_ASSERT_EQUAL(&pm, clbk.stubClbkData->lastResponder);

            TEST_ASSERT_EQUAL(12, clbk.stubClbkData->lastReceived.n1);
            TEST_ASSERT_EQUAL(33554433, clbk.stubClbkData->lastReceived.n2);

            stream.setReadUntil(35);
            pm.update();

            TEST_ASSERT_EQUAL(i*2+1, clbk.stubClbkData->callCount);

            stream.setReadUntil(49);
            pm.update();

            TEST_ASSERT_EQUAL(i*2+2, clbk.stubClbkData->callCount);
            TEST_ASSERT_EQUAL(123, clbk.stubClbkData->lastReceivedHeader.source);
            TEST_ASSERT_EQUAL(116, clbk.stubClbkData->lastReceivedHeader.destination);
            TEST_ASSERT_EQUAL(5, clbk.stubClbkData->lastReceivedHeader.id);
            TEST_ASSERT_EQUAL(1024, clbk.stubClbkData->lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::REQUEST, clbk.stubClbkData->lastReceivedHeader.direction);

            TEST_ASSERT_EQUAL(&pm, clbk.stubClbkData->lastResponder);

            TEST_ASSERT_EQUAL(12, clbk.stubClbkData->lastReceived.n1);
            TEST_ASSERT_EQUAL(15, clbk.stubClbkData->lastReceived.n2);

            stream.seekInbound(0);

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

    void test_protocol_manager_inbound_unknown_message_type(void) {
        uint8_t inboundData[] = {
            // Noise
            13,178,
            // Message 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            115,0,116,2,5,1,12,0,SerialComm::Message::MessageDirection::REQUEST,
            12,1,0,0,2,
            // Noise
            0b10101010,0b10101010,23,55,0b10101010,0b10101010,0b10101010,0b10101010,11,
            // Message 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            123,0,116,0,5,0,0,4,SerialComm::Message::MessageDirection::REQUEST,
            12,15,0,0,0
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
            []()->Time::millisecond_t{return 0;},
            100
        );
        SerialComm::ModuloNMessageIdManager midmng(42);

        StubCallback clbk;
        StubCallback clbkUnknown;

        mtm.add(
            1024,
            SerialComm::MessageManagers::createDirectMemoryMessageManager(
                sizeof(Data)
            ),
            &clbk
        );

        SerialComm::Message::registerUnknownMessageManager(&mtm, &clbkUnknown);

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

        TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);

        stream.setReadUntil(12);
        pm.update();

        TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);

        stream.setReadUntil(25);
        pm.update();

        TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);
        TEST_ASSERT_EQUAL(1, clbkUnknown.stubClbkData->callCount);

        TEST_ASSERT_EQUAL(115, clbkUnknown.stubClbkData->lastReceivedHeader.source);
        TEST_ASSERT_EQUAL(628, clbkUnknown.stubClbkData->lastReceivedHeader.destination);
        TEST_ASSERT_EQUAL(261, clbkUnknown.stubClbkData->lastReceivedHeader.id);
        TEST_ASSERT_EQUAL(12, clbkUnknown.stubClbkData->lastReceivedHeader.type);
        TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::REQUEST, clbkUnknown.stubClbkData->lastReceivedHeader.direction);

        TEST_ASSERT_EQUAL(&pm, clbkUnknown.stubClbkData->lastResponder);

        TEST_ASSERT_EQUAL(nullptr, clbkUnknown.stubClbkData->dataPtr);

        stream.setReadUntil(35);
        pm.update();

        TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);

        stream.setReadUntil(49);
        pm.update();

        TEST_ASSERT_EQUAL(1, clbk.stubClbkData->callCount);
        TEST_ASSERT_EQUAL(123, clbk.stubClbkData->lastReceivedHeader.source);
        TEST_ASSERT_EQUAL(116, clbk.stubClbkData->lastReceivedHeader.destination);
        TEST_ASSERT_EQUAL(5, clbk.stubClbkData->lastReceivedHeader.id);
        TEST_ASSERT_EQUAL(1024, clbk.stubClbkData->lastReceivedHeader.type);
        TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::REQUEST, clbk.stubClbkData->lastReceivedHeader.direction);

        TEST_ASSERT_EQUAL(&pm, clbk.stubClbkData->lastResponder);

        TEST_ASSERT_EQUAL(12, clbk.stubClbkData->lastReceived.n1);
        TEST_ASSERT_EQUAL(15, clbk.stubClbkData->lastReceived.n2);

        stream.seekInbound(0);
    }

    void test_protocol_manager_inbound_unknown_message_type_no_callback(void) {
        uint8_t inboundData[] = {
            // Noise
            13,178,
            // Message 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            115,0,64,0,5,1,12,0,SerialComm::Message::MessageDirection::REQUEST,
            12,1,0,0,2,
            // Noise
            0b10101010,0b10101010,23,55,0b10101010,0b10101010,0b10101010,0b10101010,11,
            // Message 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            123,0,64,0,5,0,0,4,SerialComm::Message::MessageDirection::REQUEST,
            12,15,0,0,0
        };
        size_t inboundSize = sizeof(inboundData)/sizeof(uint8_t);

        size_t outboundDataSize = 14;
        uint8_t outboundData[50];
        uint8_t expectedOutboundData[] = {
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            64,0,115,0,5,1,UNKNOWN_MESSAGE_TYPE,0,SerialComm::Message::MessageDirection::RESPONSE
        };

        SerialComm::TestSupport::StubStream stream(
            inboundData,
            inboundSize,
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
            TEST_ASSERT_EQUAL(i, clbk.stubClbkData->callCount);

            stream.setReadUntil(12);
            pm.update();

            TEST_ASSERT_EQUAL(i, clbk.stubClbkData->callCount);

            stream.setReadUntil(25);
            pm.update();

            TEST_ASSERT_EQUAL(i, clbk.stubClbkData->callCount);

            for (int i = 0; i < outboundDataSize; i++) {
                TEST_ASSERT_EQUAL(expectedOutboundData[i], outboundData[i]);
            }

            stream.setReadUntil(35);
            pm.update();

            TEST_ASSERT_EQUAL(i, clbk.stubClbkData->callCount);

            stream.setReadUntil(49);
            pm.update();

            TEST_ASSERT_EQUAL(i+1, clbk.stubClbkData->callCount);
            TEST_ASSERT_EQUAL(123, clbk.stubClbkData->lastReceivedHeader.source);
            TEST_ASSERT_EQUAL(64, clbk.stubClbkData->lastReceivedHeader.destination);
            TEST_ASSERT_EQUAL(5, clbk.stubClbkData->lastReceivedHeader.id);
            TEST_ASSERT_EQUAL(1024, clbk.stubClbkData->lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::REQUEST, clbk.stubClbkData->lastReceivedHeader.direction);

            TEST_ASSERT_EQUAL(&pm, clbk.stubClbkData->lastResponder);

            TEST_ASSERT_EQUAL(12, clbk.stubClbkData->lastReceived.n1);
            TEST_ASSERT_EQUAL(15, clbk.stubClbkData->lastReceived.n2);

            stream.seekInbound(0);
            stream.seekOutbound(0);

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

    void test_protocol_manager_inbound_response(void) {

        uint8_t inboundData[] = {
            // Noise
            13,178,
            // Response 1
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            116,2,123,0,5,1,0,4,SerialComm::Message::MessageDirection::RESPONSE,
            13,1,0,0,0,
            // Noise
            0b10101010,0b10101010,23,55,0b10101010,0b10101010,0b10101010,0b10101010,11,
            // Response 2
            0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
            116,0,123,0,5,0,0,4,SerialComm::Message::MessageDirection::RESPONSE,
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

        StubCallbackData clbk1Data;
        StubCallbackData clbk2Data;

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

            StubCallback *clbk1 = new StubCallback(&clbk1Data);
            pmq.add(
                Message(
                    Header {
                        .source = 123,
                        .destination = 628,
                        .id = 261,
                        .type = 1024,
                        .direction = MessageDirection::REQUEST
                    },
                    new Data {12, 33554433},
                    new DirectMemoryDestructor()
                ),
                clbk1
            );

            StubCallback *clbk2 = new StubCallback(&clbk2Data);
            pmq.add(
                Message(
                    Header {
                        .source = 123,
                        .destination = 116,
                        .id = 5,
                        .type = 1024,
                        .direction = MessageDirection::REQUEST
                    },
                    new Data {12, 15},
                    new DirectMemoryDestructor()
                ),
                clbk2
            );

            stream.setReadUntil(12);
            pm.update();

            TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);
            TEST_ASSERT_EQUAL(i, clbk1Data.callCount);
            TEST_ASSERT_EQUAL(i, clbk2Data.callCount);


            stream.setReadUntil(25);
            pm.update();

            TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);
            TEST_ASSERT_EQUAL(i+1, clbk1Data.callCount);
            TEST_ASSERT_EQUAL(i, clbk2Data.callCount);
            TEST_ASSERT_EQUAL(628, clbk1Data.lastReceivedHeader.source);
            TEST_ASSERT_EQUAL(123, clbk1Data.lastReceivedHeader.destination);
            TEST_ASSERT_EQUAL(261, clbk1Data.lastReceivedHeader.id);
            TEST_ASSERT_EQUAL(1024, clbk1Data.lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::RESPONSE, clbk1Data.lastReceivedHeader.direction);

            TEST_ASSERT_EQUAL(&pm, clbk1Data.lastResponder);

            TEST_ASSERT_EQUAL(13, clbk1Data.lastReceived.n1);
            TEST_ASSERT_EQUAL(1, clbk1Data.lastReceived.n2);

            stream.setReadUntil(35);
            pm.update();

            TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);
            TEST_ASSERT_EQUAL(i+1, clbk1Data.callCount);
            TEST_ASSERT_EQUAL(i, clbk2Data.callCount);

            stream.setReadUntil(49);
            pm.update();

            TEST_ASSERT_EQUAL(0, clbk.stubClbkData->callCount);
            TEST_ASSERT_EQUAL(i+1, clbk1Data.callCount);
            TEST_ASSERT_EQUAL(i+1, clbk2Data.callCount);
            TEST_ASSERT_EQUAL(116, clbk2Data.lastReceivedHeader.source);
            TEST_ASSERT_EQUAL(123, clbk2Data.lastReceivedHeader.destination);
            TEST_ASSERT_EQUAL(5, clbk2Data.lastReceivedHeader.id);
            TEST_ASSERT_EQUAL(1024, clbk2Data.lastReceivedHeader.type);
            TEST_ASSERT_EQUAL(SerialComm::Message::MessageDirection::RESPONSE, clbk2Data.lastReceivedHeader.direction);

            TEST_ASSERT_EQUAL(&pm, clbk2Data.lastResponder);

            TEST_ASSERT_EQUAL(17, clbk2Data.lastReceived.n1);
            TEST_ASSERT_EQUAL(42, clbk2Data.lastReceived.n2);

            stream.seekInbound(0);

            #ifndef ARDUINO_ARCH
                yield();
            #endif
        }
    }

#endif /* E52A33A4_0918_4206_A649_7114786135B5 */
