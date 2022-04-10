#ifndef C2FFA2E7_E1BA_4B03_8D23_00E703ACF15C
#define C2FFA2E7_E1BA_4B03_8D23_00E703ACF15C

    #include <stdint.h>
    #include <Arduino.h>
    #include "../../lib/serial_comm/message/message.h"
    #include "../../lib/serial_comm/message/callback.h"
    #include "../../lib/serial_comm/message_managers/message_manager.h"
    #include "../../lib/serial_comm/responder.h"
    #include "../../lib/serial_comm/data_structures/pending_message_queue.h"
    #include "../../lib/serial_comm/message/message_size.h"

    
    struct __attribute__((packed)) Data {
        uint8_t n1;
        uint32_t n2;
    };

    struct __attribute__((packed)) SizedData {
        SerialComm::Message::Size size;
        Data d;

        SizedData() {}
        SizedData(Data d): d(d), size(sizeof(SizedData)) {}
    };

    #ifdef ARDUINO_ARCH 
        const int N = 500;
    #else
        const int N = 10000;
    #endif
    

    struct StubCallbackData {
        int callCount = 0;
        Data lastReceived = {0, 0};
        void *dataPtr = nullptr;
        SerialComm::Message::Header lastReceivedHeader = {0, 0, 0, 0, SerialComm::Message::MessageDirection::RESPONSE};
        const SerialComm::Responder *lastResponder;

        StubCallbackData(): 
            callCount(0),
            lastReceived(Data{0, 0}),
            dataPtr(nullptr),
            lastReceivedHeader(SerialComm::Message::Header {0, 0, 0, 0, SerialComm::Message::MessageDirection::RESPONSE}),
            lastResponder(nullptr) {}
    };

    class StubCallback: public SerialComm::Message::MessageCallback {
        public:
            StubCallbackData *stubClbkData;
            bool deleteData = false;

            StubCallback(StubCallbackData *data = nullptr) {
                if (data == nullptr) {
                    data = new StubCallbackData();
                    deleteData = true;
                }

                stubClbkData = data;
            }

            virtual void call(
                SerialComm::Message::Message &msg,
                SerialComm::Responder *responder
            ) {
                if (msg.getData() != nullptr) {
                    stubClbkData->lastReceived = *((Data *)msg.getData());
                }

                stubClbkData->dataPtr = msg.getData();
                
                stubClbkData->lastReceivedHeader = msg.getHeader();
                
                stubClbkData->callCount++;
                stubClbkData->lastResponder = responder;
            }

            ~StubCallback() {
                if (deleteData) {
                    delete stubClbkData;
                }
            }
    };

    struct StubDestructorData {
        int nCalls = 0;
        void *lastDestroyed = nullptr;
    };

    class StubDestructor: public SerialComm::MessageManagers::Destructor {
        public:
            StubDestructorData *data;

            StubDestructor(StubDestructorData *data): data(data) {}

            virtual void destroy(void *data) {
                this->data->nCalls++;
                this->data->lastDestroyed = data;
            }
    };

    class StubTimeoutMessageSender: public SerialComm::DataStructures::TimeoutMessageSender {
        public:
            SerialComm::Message::Message lastMessage;
            SerialComm::Message::MessageCallback *lastCallback;
            int nCalls = 0;

            virtual void sendTimeoutMessage(
                SerialComm::Message::Message originalMessage, 
                SerialComm::Message::MessageCallback *callback
            ) {

                nCalls++;
                lastMessage = originalMessage;
                lastCallback = callback;
            }
    };

#endif /* C2FFA2E7_E1BA_4B03_8D23_00E703ACF15C */
