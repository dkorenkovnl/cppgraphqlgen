#include "TestServerWrapper.h"

#include "graphqlservice/JSONResponse.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/config.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>

using namespace graphql;
using namespace std::literals;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = boost::asio::ip::tcp;
using time_point = std::chrono::steady_clock::time_point;
using steady_timer = boost::asio::steady_timer;
using boost::asio::use_awaitable;
using namespace boost::asio::experimental::awaitable_operators;
namespace this_coro = boost::asio::this_coro;

using tcp_stream = typename beast::tcp_stream::rebind_executor<net::use_awaitable_t<>::executor_with_default<net::any_io_executor>>::other;

constexpr net::string_view c_host { "127.0.0.1" };
constexpr unsigned short c_port = 8080;
constexpr beast::string_view c_target { "/graphql" };
// ws://127.0.0.1:8080/graphql
// http://127.0.0.1:8080/graphql
void exception_cb(std::exception_ptr exp)
{
  if (exp)
  {
    std::rethrow_exception(exp);
  }
}

void session_exception_cb(std::exception_ptr exp)
{
  if (exp)
  {
    try
    {
      std::rethrow_exception(exp);
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Session error: " << ex.what() << std::endl;
    }
  }
}

net::awaitable<void> watchdog(time_point& deadline) {
  steady_timer timer(co_await net::this_coro::executor);
  auto now = std::chrono::steady_clock::now();
  while (deadline > now) {
    timer.expires_at(deadline);
    co_await timer.async_wait(use_awaitable);
    now = std::chrono::steady_clock::now();
  }
}

net::awaitable<void> read(tcp_stream& stream, beast::flat_buffer& buffer, http::request<http::string_body>& req)
{
  co_await http::async_read(stream, buffer, req);
}

net::awaitable<void> client_coroutines(tcp_stream stream, std::shared_ptr<service::Request> service)
{
  std::cout << "client_coroutines" << std::endl;
  // This buffer is required to persist across reads
  beast::flat_buffer buffer;
  bool keepAlive = false;
  // This lambda is used to send messages
  try
  {
    do
    {
      // Set the timeout.
      //stream.expires_after(std::chrono::seconds(30));
      // Read a request
      http::request<http::string_body> req;
      time_point deadline{};
      //co_await  read(stream, buffer, req);
      std::cout << "http::async_read before" << std::endl;
      co_await http::async_read(stream, buffer, req);
      std::cout << "http::async_read after" << std::endl;
      // Handle the request
      http::response<http::string_body> msg;
      try
      {
//        if (req.method() != http::verb::post || req.target() != c_target)
//        {
//          std::cout << "!= http::verb::post" << std::endl;
//          throw std::runtime_error("Only POST requests to /graphql are supported.");
//        }
//        else
        {
          std::ostringstream oss;
          oss << req.body();
          auto payload = response::parseJSON(oss.str());
          if (payload.type() != response::Type::Map)
          {
            throw std::runtime_error("Invalid request!");
          }
          const auto queryItr = payload.find("subscription"sv);
          if (queryItr == payload.end() || queryItr->second.type() != response::Type::String)
          {
            throw std::runtime_error("Invalid request!");
          }
          peg::ast query;
          query = peg::parseString(queryItr->second.get<response::StringType>());

          if (!query.root)
          {
            throw std::runtime_error("Unknown error!");
          }

          const auto operationNameItr = payload.find("operationName"sv);
          const auto operationName = (operationNameItr != payload.end() && operationNameItr->second.type() == response::Type::String)
                                     ? std::string_view { operationNameItr->second.get<response::StringType>() }
                                     : std::string_view {};

          const auto variablesItr = payload.find("variables"sv);
          auto variables = (variablesItr != payload.end() && variablesItr->second.type() == response::Type::String)
                           ? response::parseJSON(operationNameItr->second.get<response::StringType>())
                           : response::Value {};

          msg = http::response<http::string_body> { http::status::ok,req.version() };
          msg.set(http::field::server, BOOST_BEAST_VERSION_STRING);
          msg.set(http::field::content_type, "application/json");
          msg.keep_alive(req.keep_alive());
          msg.body() = response::toJSON(service->resolve({ query, operationName, std::move(variables) }).get());
          msg.prepare_payload();
        }
      }
      catch (const std::runtime_error& ex)
      {
        msg = http::response<http::string_body> {http::status::bad_request,req.version()};
        msg.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        msg.set(http::field::content_type, "text/plain");
        msg.keep_alive(req.keep_alive());
        msg.body() = "Error: "s + ex.what();
        msg.prepare_payload();
      }
      // Determine if we should close the connection
      keepAlive = msg.keep_alive();
      // Send the response
      //co_await beast::async_write(stream,http::message_generator { std::move(msg) },net::use_awaitable);
    } while (keepAlive);
  }
  catch (boost::system::system_error& se)
  {
    std::cout << "catch (boost::system::system_error& se)" << std::endl;
    std::cout << se.what() << std::endl;
    if (se.code() != http::error::end_of_stream)
      throw;
  }
  // Send a TCP shutdown
  beast::error_code ec;
  stream.socket().shutdown(tcp::socket::shutdown_send, ec);
  // At this point the connection is closed gracefully
  // we ignore the error because the client might have
  // dropped the connection already.
  std::cout << "stop" << std::endl;
}

net::awaitable<void> accept_coroutines(tcp::endpoint endpoint, std::shared_ptr<service::Request> service)
{
// Open the acceptor.
  auto acceptor = net::use_awaitable.as_default_on(tcp::acceptor(co_await net::this_coro::executor));
  acceptor.open(endpoint.protocol());
  // Allow address reuse.
  acceptor.set_option(net::socket_base::reuse_address(true));
  // Bind to the server address.
  acceptor.bind(endpoint);
  // Start listening for connections.
  acceptor.listen(net::socket_base::max_listen_connections);
  while (true)
  {
    net::co_spawn(acceptor.get_executor(),client_coroutines(tcp_stream(co_await acceptor.async_accept()), service),session_exception_cb);
  }
}

int main(int , char** )
{
  const auto server = test_server::create_test_server();
  const auto& service = server->service;

  std::cout << "Created the test_server_web_subscription..." << std::endl;
  try
  {
    auto query = peg::parseString(R"(subscription {
		subscriptionTelemetry {
			id: id
			when
			name
			sensorId
			sensorValue
		}
	})");
    const auto address = net::ip::make_address(c_host);
    // The io_context is required for all I/O.
    net::io_context ioc;

    // Spawn a listening port.
    net::co_spawn(ioc,accept_coroutines(tcp::endpoint(address, c_port), std::move(service)),exception_cb);
    try
    {
      ioc.run();
    }
    catch (const std::runtime_error& ex)
    {
      std::cerr << ex.what() << std::endl;
      return 1;
    }

    response::Value variables(response::Type::Map);
    auto state = std::make_shared<test_server::RequestState>(1);

    response::Value result;
    auto key = service->subscribe({ [&result](response::Value&& response) {
        result = std::move(response);
    },
                                    std::move(query),
                                    ""s,
                                    std::move(variables),
                                    {},
                                    state }).get();
    service->deliver({ "subscriptionTelemetry"sv }).get();
    service->unsubscribe({ key }).get();

    std::cout << response::toJSON(response::Value(result)) << std::endl;

  }
  catch (const std::runtime_error& ex)
  {
    std::cerr << ex.what() << std::endl;
    return 1;
  }
  return 0;
}