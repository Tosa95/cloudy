#ifndef CB988BD9_2510_47C4_8167_06A2447FE46D
#define CB988BD9_2510_47C4_8167_06A2447FE46D

    #include "../../lib/logging/sinks/log_sink.h"
    #include "../../lib/linked_list/LinkedList.h"

    class StubSink: public Logging::LogSink {
        public:
            LinkedList2::LinkedList<Logging::LogEntry> entriesReceived;
            int nCalls = 0;
            virtual void put(Logging::LogEntry &entry) {
                nCalls++;
                entriesReceived.add(entry.copy());
            }

            void clear() {
                while (entriesReceived.size() > 0) {
                    entriesReceived.get(0).deleteData();
                    entriesReceived.remove(0);
                    nCalls = 0;
                }
            }
    };

#endif /* CB988BD9_2510_47C4_8167_06A2447FE46D */
