/** --------------------------------------------------------------------------
 *  WebSocketServer.cpp
 *
 *  Base class that WebSocket implementations must inherit from.  Handles the
 *  client connections and calls the child class callbacks for connection
 *  events like onConnect, onMessage, and onDisconnect.
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  --------------------------------------------------------------------------
 **/

#include <string>
#include <cstring>
#include <stdexcept>
#include "libwebsockets.h"
#include "Util.h"
#include "WebSocketServer.h"
#include "log.h"

using namespace std;

// 0 for unlimited
#define MAX_BUFFER_SIZE 0
std::stringstream msgWS;

// Nasty hack because certain callbacks are statically defined
WebSocketServer *self;

static int callback_main(   struct lws *wsi,
                            enum lws_callback_reasons reason,
                            void *user,
                            void *in,
                            size_t len )
{
    int fd;

    switch( reason ) {
        case LWS_CALLBACK_ESTABLISHED:
            self->onConnectWrapper( lws_get_socket_fd( wsi ) );
            lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            fd = lws_get_socket_fd( wsi );
            while( !self->connections[fd]->buffer.empty( ) )
            {
                const char * message = self->connections[fd]->buffer.front( );
                size_t msgLen = strlen(message);
                int charsSent = lws_write( wsi, (unsigned char *)message, msgLen, LWS_WRITE_TEXT );
                if( charsSent < msgLen )
                    self->onErrorWrapper( fd, string( "Error writing to socket" ) );
                else
                    // Only pop the message if it was sent successfully.
                    self->connections[fd]->buffer.pop_front( );
            }
            lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_RECEIVE:
            self->onMessage( lws_get_socket_fd( wsi ), string( (const char *)in, len ) );
            break;

        case LWS_CALLBACK_CLOSED:
            self->onDisconnectWrapper( lws_get_socket_fd( wsi ) );
            break;

        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "/",
        callback_main,
        0, // user data struct not used
        MAX_BUFFER_SIZE,
    },{ nullptr, nullptr, 0, 0 } // terminator
};

WebSocketServer::WebSocketServer( int port, const string certPath, const string& keyPath )
{
    this->_port     = port;
    this->_certPath = certPath;
    this->_keyPath  = keyPath;

    lws_set_log_level( 0, lwsl_emit_syslog ); // We'll do our own logging, thank you.
    struct lws_context_creation_info info{};
    memset( &info, 0, sizeof info );
    info.port = this->_port;
    info.iface = nullptr;
    info.protocols = protocols;
#ifndef LWS_NO_EXTENSIONS
    //info.extensions = lws_get_internal_extensions( );
#endif

    if( !this->_certPath.empty( ) && !this->_keyPath.empty( ) )
    {
        Util::log( "Using TLS certPath=" + this->_certPath + ". keyPath=" + this->_keyPath + "." );
        info.ssl_cert_filepath        = this->_certPath.c_str( );
        info.ssl_private_key_filepath = this->_keyPath.c_str( );
    }
    else
    {
        Util::log( "Not using TLS" );
        info.ssl_cert_filepath        = nullptr;
        info.ssl_private_key_filepath = nullptr;
    }
    info.gid = -1;
    info.uid = -1;
    info.options = 0;

    // keep alive
    info.ka_time = 60; // 60 seconds until connection is suspicious
    info.ka_probes = 10; // 10 probes after ^ time
    info.ka_interval = 10; // 10s interval for sending probes
    this->_context = lws_create_context( &info );
    if( !this->_context )
        throw runtime_error("libwebsocket init failed");
    msgWS.str("");
    msgWS << "Server started on port " << Util::toString( this->_port ) << endl;
    Logger(msgWS.str());

    // Some of the libwebsocket stuff is define statically outside the class. This
    // allows us to call instance variables from the outside.  Unfortunately this
    // means some attributes must be public that otherwise would be private.
    self = this;
}

WebSocketServer::~WebSocketServer( )
{
    // Free up some memory
    for( auto it = this->connections.begin( ); it != this->connections.end( ); ++it )
    {
        Connection* c = it->second;
        this->connections.erase( it->first );
        delete c;
    }
}

void WebSocketServer::onConnectWrapper( int socketID )
{
    auto* c = new Connection;
    c->createTime = time( nullptr );
    this->connections[ socketID ] = c;
    this->onConnect( socketID );
}

void WebSocketServer::onDisconnectWrapper( int socketID )
{
    this->onDisconnect( socketID );
    this->_removeConnection( socketID );
}

void WebSocketServer::onErrorWrapper( int socketID, const string& message )
{
    Util::log( "Error: " + message + " on socketID '" + Util::toString( socketID ) + "'" );
    this->onError( socketID, message );
    this->_removeConnection( socketID );
}

void WebSocketServer::send( int socketID, string data )
{
    // Push this onto the buffer. It will be written out when the socket is writable.
    this->connections[socketID]->buffer.push_back( data.c_str() );
}

void WebSocketServer::broadcast(const string& data )
{
    for(auto &connection : this->connections)
        this->send( connection.first, data );
}

void WebSocketServer::setValue( int socketID, const string& name, const string& value )
{
    this->connections[socketID]->keyValueMap[name] = value;
}

string WebSocketServer::getValue( int socketID, const string& name )
{
    return this->connections[socketID]->keyValueMap[name];
}
size_t WebSocketServer::getNumberOfConnections( )
{
    return this->connections.size();
}

void WebSocketServer::run( uint64_t timeout )
{
    while( true )
    {
        this->wait( timeout );
    }
}

void WebSocketServer::wait( uint64_t timeout )
{
    if( lws_service( this->_context, timeout ) < 0 )
        throw runtime_error("Error polling for socket activity.");
}

void WebSocketServer::_removeConnection( int socketID )
{
    Connection* c = this->connections[ socketID ];
    this->connections.erase( socketID );
    delete c;
}
