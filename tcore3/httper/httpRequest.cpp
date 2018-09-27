#include "httpRequest.h"
#include "httper.h"

namespace tcore {
    tlib::tpool<httpRequest> s_request_pool;

    httpRequest * httpRequest::create(const s64 index, const s32 id, const char * url, iHttpResponse * response, const iContext & context) {
        return create_from_pool(s_request_pool, index, id, url, response, context);
    }

    void httpRequest::release() {
        recover_to_pool(s_request_pool, this);
    }

    void httpRequest::postParam(const char * key, const char * value) {
        if (_params.length() != 0) {
            _params.append("&");
        }

        _params.append(key).append("=").append(value);
    }

    void httpRequest::doRequest() {
        _reponse->_reference++;
        httper::getInstance()->push(this);
    }

    void httpRequest::onResponse() {
        if (nullptr == _reponse) {
            return;
        }

        if (_error != 0) {
            _reponse->onError(_id, _error, _context);
        }
        else {
            s32 len;
            _reponse->_reference--;
            const void * data = _data.getBuff(len);
            _reponse->onResponse(_id, data, len, _context);
        }
    }
}
