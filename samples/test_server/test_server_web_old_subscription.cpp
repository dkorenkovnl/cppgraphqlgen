#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "TestServerWrapper.h"
#include "graphqlservice/JSONResponse.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace boost::json;
using namespace graphql;
using namespace std::literals;

void do_session(tcp::socket socket)
{
  try
  {
    // Construct the stream by moving in the socket
    websocket::stream<tcp::socket> ws{std::move(socket)};

    ws.set_option(
            websocket::stream_base::timeout::suggested(
                    beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-sync");
            }));

    // Accept the websocket handshake
    ws.accept();

    // This buffer will hold the incoming message
    beast::flat_buffer buffer;
    // Read a message
    error_code ec;
    ws.read(buffer,ec);
    if(ec.failed())
    {
      std::cout << "failed to read from socket" << std::endl;
      return;
    }
    auto req_txt = beast::buffers_to_string(buffer.data());
    value req_val = parse(req_txt);
    object obj = req_val.as_object();
    std::string query_str = obj["query"].as_string().c_str();
    std::cout << query_str << std::endl;
    peg::ast ast = peg::parseString(query_str);

    const auto server = test_server::create_test_server();
    const auto& service = server->service;

    response::Value variables(response::Type::Map);
    auto state = std::make_shared<test_server::RequestState>(1);

    response::Value result;
    auto key = service->subscribe({ [&result](response::Value&& response) {
        result = std::move(response);
    },
                                    std::move(ast),
                                    ""s,
                                    std::move(variables),
                                    {},
                                    state }).get();

    for(;;)
    {
      service->deliver({ "subscriptionTelemetry"sv }).get();
      auto resp_txt = response::toJSON(response::Value(result));
      std::cout << resp_txt << std::endl;
      auto sz = ws.write(boost::asio::buffer(resp_txt),ec);
      std::cout << "sz " << sz << std::endl;
      if (ec)
      {
        std::cout << "failed to write from socket" << std::endl;
        break;
      }
      std::this_thread::sleep_for(1000ms);
    }
    service->unsubscribe({ key }).get();

  }
  catch(beast::system_error const& se)
  {
    // This indicates that the session was closed
    if(se.code() != websocket::error::closed)
      std::cerr << "Error: " << se.code().message() << std::endl;
  }
  catch(std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

int main(int , char**)
{
  try
  {
    auto const address = net::ip::make_address("127.0.0.1");
    auto const port = static_cast<unsigned short>(9090);

    // The io_context is required for all I/O
    net::io_context ioc{1};

    // The acceptor receives incoming connections
    tcp::acceptor acceptor{ioc, {address, port}};
    for(;;)
    {
      // This will receive the new connection
      tcp::socket socket{ioc};

      // Block until we get a connection
      acceptor.accept(socket);

      // Launch the session, transferring ownership of the socket
      std::thread(
              &do_session,
              std::move(socket)).detach();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
