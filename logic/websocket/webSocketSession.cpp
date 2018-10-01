#include "webSocketSession.h"

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

bool webSocketSession::tryShake(const void * data, const s32 size) {
    std::istringstream s((char *)data);
    std::string request;

    std::getline(s, request);
    if (request.size() >= 1 && request[request.size() - 1] == '\r') {
        request.erase(request.end() - 1);
    } else {
        return false;
    }

    std::string header;
    std::string::size_type end;

    while (std::getline(s, header) && header != "\r") {
        if (header[header.size() - 1] != '\r') {
            return false;
        }
        else {
            header.erase(header.end() - 1);    //remove last char
        }

        end = header.find(": ", 0);
        if (end != std::string::npos) {
            std::string key = header.substr(0, end);
            std::string value = header.substr(end + 2);
            _header_map[key] = value;
        }
    }

    string response = "HTTP/1.1 101 Switching Protocols\r\n";
    response += "Connection: upgrade\r\n";
    response += "Sec-webSocket-Accept: ";
    std::string server_key = _header_map["Sec-webSocket-Key"];
    server_key += MAGIC_KEY;

    tlib::sha1 sha;
    unsigned int message_digest[5];
    sha.reset();
    sha << server_key.c_str();

    sha.result(message_digest);
    for (int i = 0; i < 5; i++) {
        message_digest[i] = htonl(message_digest[i]);
    }
    server_key = tlib::base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
    server_key += "\r\n";
    response += server_key.c_str();
    response += "Upgrade: websocket\r\n\r\n";

    send(response.c_str(), response.size());
    return true;
}

void webSocketSession::fetchFin(char *msg, int &pos) {
    _fin = (unsigned char)msg[pos] >> 7;
}

void webSocketSession::fetchOpcode(char *msg, int &pos) {
    _opcode = msg[pos] & 0x0f;
    pos++;
}

void webSocketSession::fetchMask(char *msg, int &pos) {
    _mask = (unsigned char)msg[pos] >> 7;
}

void webSocketSession::fetchMaskingKey(char *msg, int &pos) {
    if (_mask != 1)
        return;
    for (int i = 0; i < 4; i++)
        _masking_key[i] = msg[pos + i];
    pos += 4;
}

void webSocketSession::fetchPayloadLength(char *msg, int &pos) {
    _payload_length = msg[pos] & 0x7f;
    pos++;
    if (_payload_length == 126) {
        u16 length = 0;
        memcpy(&length, msg + pos, 2);
        pos += 2;
        _payload_length = ntohs(length);
    } else if (_payload_length == 127) {
        u32 length = 0;
        memcpy(&length, msg + pos, 4);
        pos += 4;
        _payload_length = ntohl(length);
    }
}

void webSocketSession::fetchPayload(char *msg, int &pos) {
    memset(_payload, 0, sizeof(_payload));
    if (_mask != 1) {
        memcpy(_payload, msg + pos, _payload_length);
    } else {
        for (u32 i = 0; i < _payload_length; i++) {
            int j = i % 4;
            _payload[i] = msg[pos + i] ^ _masking_key[j];
        }
    }
    pos += _payload_length;
}

void webSocketSession::reset() {
    _fin = 0;
    _opcode = 0;
    _mask = 0;
    memset(_masking_key, 0, sizeof(_masking_key));
    _payload_length = 0;
    memset(_payload, 0, sizeof(_payload));
}

webSocketSession::webSocketSession(iWBSessionDelegate * delegate) : _delegate(delegate), _shaked(false) {
    iWBSession * session = this;
    tools::memery::safeMemcpy((void*)&(_delegate->_session), sizeof(_delegate->_session), (iWBSession *)&session, sizeof(this));
}

void webSocketSession::sendmessage(const void * data, const s32 len, const eFrameType frametype) {
    if (_shaked) {
        char * buffer = (char *)alloca(len + 6);
        int pos = 0;
        int size = len;
        buffer[pos++] = frametype; // text frame
        if (size <= 125) {
            buffer[pos++] = size;
        } else if (size <= 65535) {
            buffer[pos++] = 126;

            buffer[pos++] = (size >> 8) & 0xFF;
            buffer[pos++] = size & 0xFF;
        } else {
            buffer[pos++] = 127;
            for (int i = 3; i >= 0; i--) {
                buffer[pos++] = 0;
            }
            for (int i = 3; i >= 0; i--) {
                buffer[pos++] = ((size >> 8 * i) & 0xFF);
            }
        }
        memcpy((void*)(buffer + pos), data, size);
        send(buffer, size + pos);
    }
}

#define PROTO_HEADER_LEN 6
int webSocketSession::onRecv(api::iCore * core, const void * context, const int size) {
    if (!_shaked) {
        if (tryShake(context, size)) {
            _shaked = true;
            _delegate->onConnect();
            return size;
        } else {
            close();
        }
    } else {
        if (size < PROTO_HEADER_LEN) {
            return 0;
        }

        reset();

        int pos = 0;
        fetchFin((char *)context, pos);
        fetchOpcode((char *)context, pos);
        fetchMask((char *)context, pos);
        fetchPayloadLength((char *)context, pos);
        if (_payload_length > size - PROTO_HEADER_LEN) {
            return 0;
        }

        fetchMaskingKey((char *)context, pos);
        fetchPayload((char *)context, pos);

        _delegate->onRecv(_payload, _payload_length);
        return pos;
    }

    return 0;
}

void webSocketSession::onConnected(api::iCore * core) {
    
}

void webSocketSession::onDisconnect(api::iCore * core) {
    iWBSession * session = nullptr;
    tools::memery::safeMemcpy((void*)&(_delegate->_session), sizeof(_delegate->_session), (iWBSession *)&session, sizeof(this));
    _delegate->onDisconnect();
    DEL this;
}

void webSocketSession::onConnectFailed(api::iCore * core) {

}
