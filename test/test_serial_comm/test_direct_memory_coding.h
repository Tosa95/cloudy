#ifndef E5C9FAAB_E17C_4861_AE25_9CC2AFFEFBE1
#define E5C9FAAB_E17C_4861_AE25_9CC2AFFEFBE1

    #include <unity.h>

    #include "../../lib/serial_comm/message_managers/serializers/direct_memory_serializer.h"
    #include "../../lib/serial_comm/message_managers/serializers/size_agnostic_direct_memory_serializer.h"
    #include "../../lib/serial_comm/message_managers/deserializers/direct_memory_deserializer.h"
    #include "../../lib/serial_comm/message_managers/deserializers/size_agnostic_direct_memory_deserializer.h"
    #include "../../lib/serial_comm/message_managers/destructors/direct_memory_destructor.h"
    #include "../../lib/serial_comm/message_managers/destructors/no_destructor.h"
    #include "common.h"

    using namespace SerialComm::MessageManagers;

    void test_direct_memory_serialize(void) {
        
        for (int k = 0; k < N; k++) {
            Data d = {
                .n1 = 12,
                .n2 = 33554433
            };

            uint8_t dataSerialized[30];

            DirectMemorySerializerFactory serFact(sizeof(d));

            Serializer *dms = serFact.create(&d);

            uint8_t *curr;
            int i = 0;

            while ((curr = dms->next()) != nullptr) {
                dataSerialized[i++] = *curr;
            }

            free(dms);

            TEST_ASSERT_EQUAL(5, i);

            TEST_ASSERT_EQUAL(12, dataSerialized[0]);
            TEST_ASSERT_EQUAL(1, dataSerialized[1]);
            TEST_ASSERT_EQUAL(0, dataSerialized[2]);
            TEST_ASSERT_EQUAL(0, dataSerialized[3]);
            TEST_ASSERT_EQUAL(2, dataSerialized[4]);

            #ifdef ESP8266_ARCH 
                yield();
            #endif
        }

    }

    void test_direct_memory_deserialize(void) {
        
        for (int k = 0; k < N; k++) {
            Data d;

            uint8_t dataSerialized[5] = {12,1,0,0,2};

            DirectMemoryDeserializerFactory desFact(sizeof(Data), &d);

            Deserializer *dmd = desFact.create();

            int i = 0;

            while (dmd->put(dataSerialized[i])) {
                i++;
            }

            free(dmd);

            // i is how many times put returned true, that must be sizeof(data) - 1
            TEST_ASSERT_EQUAL(4, i);

            TEST_ASSERT_EQUAL(12, d.n1);
            TEST_ASSERT_EQUAL(33554433, d.n2);

            NoDestructorFactory destFact;

            Destructor *dest = destFact.create();

            dest->destroy(&d);

            free(dest);

            #ifdef ESP8266_ARCH 
                yield();
            #endif
        }

    }

    void test_direct_memory_deserialize_heap(void) {

        for (int k = 0; k < N; k++) {
            uint8_t dataSerialized[5] = {12,1,0,0,2};

            DirectMemoryDeserializerFactory desFact(sizeof(Data));

            Deserializer *dmd = desFact.create();

            int i = 0;

            while (dmd->put(dataSerialized[i])) {
                i++;
            }

            Data *d = (Data *)dmd->getData();

            free(dmd);

            // i is how many times put returned true, that must be sizeof(data) - 1
            TEST_ASSERT_EQUAL(4, i);

            TEST_ASSERT_EQUAL(12, d->n1);
            TEST_ASSERT_EQUAL(33554433, d->n2);

            DirectMemoryDestructorFactory destFact;
            Destructor *dest = destFact.create();

            dest->destroy(d);

            free(dest);

            #ifdef ESP8266_ARCH 
                yield();
            #endif
        }

        
    }

    void test_size_agnostic_direct_memory_serialize(void) {

        for (int k = 0; k < N; k++) {
            SizedData sd(Data {.n1 = 12, .n2 = 33554433});

            uint8_t dataSerialized[30];

            SizeAgnosticDirectMemorySerializerFactory serFact;

            Serializer *dms = serFact.create(&sd);

            uint8_t *curr;
            int i = 0;

            while ((curr = dms->next()) != nullptr) {
                dataSerialized[i++] = *curr;
            }

            free(dms);

            TEST_ASSERT_EQUAL(7, i);

            TEST_ASSERT_EQUAL(sizeof(SizedData), dataSerialized[0]);
            TEST_ASSERT_EQUAL(0, dataSerialized[1]);
            TEST_ASSERT_EQUAL(12, dataSerialized[2]);
            TEST_ASSERT_EQUAL(1, dataSerialized[3]);
            TEST_ASSERT_EQUAL(0, dataSerialized[4]);
            TEST_ASSERT_EQUAL(0, dataSerialized[5]);
            TEST_ASSERT_EQUAL(2, dataSerialized[6]);

            #ifdef ESP8266_ARCH 
                yield();
            #endif
        }

        

    }

    void test_size_agnostic_direct_memory_deserialize(void) {

        for (int k = 0; k < N; k++) {
            SizedData sd;

            uint8_t dataSerialized[7] = {sizeof(SizedData), 0, 12, 1, 0, 0, 2};

            SizeAgnosticDirectMemoryDeserializerFactory desFact(&sd);

            Deserializer *dmd = desFact.create();

            int i = 0;

            while (dmd->put(dataSerialized[i])) {
                i++;
            }

            free(dmd);

            // i is how many times put returned true, that must be sizeof(data) - 1
            TEST_ASSERT_EQUAL(6, i);

            TEST_ASSERT_EQUAL(sizeof(SizedData), sd.size);
            TEST_ASSERT_EQUAL(12, sd.d.n1);
            TEST_ASSERT_EQUAL(33554433, sd.d.n2);

            NoDestructorFactory destFact;

            Destructor *dest = destFact.create();

            dest->destroy(&sd);

            free(dest);

            #ifdef ESP8266_ARCH 
                yield();
            #endif
        }

    }

    void test_size_agnostic_direct_memory_deserialize_heap(void) {

        for (int k = 0; k < N; k++) {

            uint8_t dataSerialized[7] = {sizeof(SizedData), 0, 12, 1, 0, 0, 2};

            SizeAgnosticDirectMemoryDeserializerFactory desFact;

            Deserializer *dmd = desFact.create();

            int i = 0;

            while (dmd->put(dataSerialized[i])) {
                i++;
            }

            SizedData *sd = (SizedData *)dmd->getData();

            free(dmd);

            // i is how many times put returned true, that must be sizeof(data) - 1
            TEST_ASSERT_EQUAL(6, i);

            TEST_ASSERT_EQUAL(sizeof(SizedData), sd->size);
            TEST_ASSERT_EQUAL(12, sd->d.n1);
            TEST_ASSERT_EQUAL(33554433, sd->d.n2);

            DirectMemoryDestructorFactory destFact;
            Destructor *dest = destFact.create();

            dest->destroy(sd);

            free(dest);

            #ifdef ESP8266_ARCH 
                yield();
            #endif
        }

    }

#endif /* E5C9FAAB_E17C_4861_AE25_9CC2AFFEFBE1 */
