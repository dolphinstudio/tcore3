#include "logger.h"
#include "tools.h"
#ifdef WIN32
#include <windows.h>
#endif //WIN32

namespace tcore {
#define time_out_for_cut_file 5 * 60 * 60 * 1000
#define read_count 64

    logger * logger::getInstance() {
        static logger * s_logger = nullptr;
        if (nullptr == s_logger) {
            s_logger = NEW logger;
            s_logger->launch();
        }

        return s_logger;
    }

    bool logger::launch() {
        setSyncFilePrefix(tools::int64AsString(tools::system::getCurrentProcessId()).c_str());
        setAsyncFilePrefix(tools::int64AsString(tools::system::getCurrentProcessId()).c_str());

        start();
        _thread_status = STATUS_STARTING;
        return true;
    }


    void logger::shutdown() {
        terminate();
        DEL this;
    }

    void logger::logSync(const s64 tick, const char * log, const bool echo) {
        std::string timestr = tools::time::getTimeString(tick);
        _sync_file.Write(timestr.c_str());
        _sync_file.Write("|");
        _sync_file.Write(log);
        _sync_file.Write("\n");
        _sync_file.Flush();

        if (echo) {
            printf(timestr.c_str());
            printf(" | ");
            printf("%s\n", log);
        }
    }

    void logger::logAsync(const s64 tick, const char * log, const bool echo) {
        while (_queue[_write_index]._has_data) {
            msleep(1);
        }

        _queue[_write_index]._tick = tick;
        _queue[_write_index]._log = log;
        _queue[_write_index]._echo = echo;
        _queue[_write_index]._has_data = true;

        _write_count++;
        if (++_write_index >= log_queue_max_size) {
            _write_index = 0;
        }
    }

    void logger::setSyncFilePrefix(const char * prefix) {
        _sync_prefix = prefix;
        if (_sync_file.IsOpen()) {
            _sync_file.Close();
        }

        std::string path = tools::file::getApppath();
        path.append("/log/");

        tools::file::mkdir(path.c_str());
        tassert(!_sync_file.IsOpen(), "sync log file created, fuck");

        if (!_sync_file.IsOpen()) {
            std::string name;
            name.append(_sync_prefix).append("_").append(tools::time::getCurrentTimeString("sync_%4d_%02d_%02d_%02d_%02d_%02d.log"));

            if (!_sync_file.Open(path.c_str(), name.c_str())) {
                tassert(false, "wtf");
                printf("open log file : %s error\n", name.c_str());
            }
        }
    }

    void logger::setAsyncFilePrefix(const char * prefix) {
        _async_prefix = prefix;
        if (_async_file.IsOpen()) {
            _async_file.Close();
        }

        std::string path = tools::file::getApppath();
        path.append("/log/");

        tools::file::mkdir(path.c_str());
        tassert(!_async_file.IsOpen(), "sync log file created, fuck");

        std::string name;
        name.append(_async_prefix.c_str()).append("_").append(tools::time::getCurrentTimeString("async_%4d_%02d_%02d_%02d_%02d_%02d.log"));

        if (!_async_file.Open(path.c_str(), name.c_str())) {
            tassert(false, "wtf");
            printf("open log file : %s error\n", name.c_str());
            return;
        }
    }

    void logger::terminate() {
        tassert(_thread_status == STATUS_STARTING, "wtf");
        if (_thread_status == STATUS_STARTING) {
            _thread_status = STATUS_STOPPING;
            while (_thread_status != STATUS_STOPPED) {
                msleep(1);
            }
        }
    }

    void logger::run() {
        while (true) {
            while (!_queue[_read_index]._has_data) {
                if (_read_count == _write_count) {
                    if (_thread_status == STATUS_STOPPING) {
                        _thread_status = STATUS_STOPPED;
                        return;
                    }
                }
                msleep(1);
            }

            while (_async_prefix == "") {
                msleep(100);
            }

            if (!_async_file.IsOpen()) {
                std::string name;
                name.append(_async_prefix).append("_").append(tools::time::getCurrentTimeString("async_%4d_%02d_%02d_%02d_%02d_%02d.log"));

                std::string path;
                path.append(tools::file::getApppath()).append("/log/");

                if (!_async_file.Open(path.c_str(), name.c_str())) {
                    tassert(false, "wtf");
                    printf("open log file : %s error\n", name.c_str());
                    return;
                }
            }
            
            s64 tick = tools::time::getMillisecond();
            if (tick - _async_file.GetOpenTick() >= time_out_for_cut_file) {
                _async_file.Close();
                std::string name;
                name.append( _async_prefix).append("_").append(tools::time::getCurrentTimeString("async_%4d_%02d_%02d_%02d_%02d_%02d.log"));

                std::string path;
                path.append(tools::file::getApppath()).append("/log/");

                if (!_async_file.Open(path.c_str(), name.c_str())) {
                    tassert(false, "wtf");
                    printf("open log file : %s error\n", name.c_str());
                    return;
                }
                printf("log file cut %lld\n", tick);
            }

            s32 count = 0;
            while (_queue[_read_index]._has_data && count++ <= read_count) {
                _async_file.Write(tools::time::getTimeString(_queue[_read_index]._tick).c_str());
                _async_file.Write("|");
                _async_file.Write(_queue[_read_index]._log.c_str());
                _async_file.Write("\n");

                if (_queue[_read_index]._echo) {
                    printf("%s|%s\n", tools::time::getTimeString(_queue[_read_index]._tick).c_str(), _queue[_read_index]._log.c_str());
                }

                _queue[_read_index].clear();
                _read_index++;
                _read_count++;

                if (_read_index >= log_queue_max_size) {
                    _read_index = 0;
                }
            }

            _async_file.Flush();
        }
    }
}
