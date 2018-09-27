#ifndef __logger_h__
#define __logger_h__
#include "logfile.h"
#include "cthread.h"

#define log_queue_max_size 10240

namespace tcore {
    class logger : public tlib::cthread {
        struct logunit {
            s64 _tick;
            std::string _log;
            bool _echo;
            bool _has_data;

            logunit() : _tick(0), _log(""), _echo(false), _has_data(false) {}

            void clear() {
                _tick = 0;
                _log.clear();
                _echo = false;
                _has_data = false;
            }
        };

    public:
        static logger * getInstance();


        virtual bool launch();
        virtual void proces(const s32 overtiem) {}
        virtual void shutdown();

        virtual void logSync(const s64 tick, const char * log, const bool echo);
        virtual void logAsync(const s64 tick, const char * log, const bool echo);
        virtual void setSyncFilePrefix(const char * prefix);
        virtual void setAsyncFilePrefix(const char * prefix);

        virtual void terminate();
        virtual void run();

    private:
        logger() :
            _read_index(0),
            _write_index(0),
            _read_count(0),
            _write_count(0),
            _thread_status(STATUS_UNINITIALIZE) {
            _sync_prefix.clear();
            _async_prefix.clear();
        }

    private:
        logunit _queue[log_queue_max_size];
        s32 _read_index;
        s32 _read_count;
        s32 _write_index;
        s32 _write_count;

        std::string _sync_prefix;
        logfile _sync_file;

        std::string _async_prefix;
        logfile _async_file;

        s8 _thread_status;
        enum {
            STATUS_UNINITIALIZE = 0,
            STATUS_STARTING = 1,
            STATUS_STOPPING = 2,
            STATUS_STOPPED = 3,
        };
    };
}
#endif //__logger_h__
