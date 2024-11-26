#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


//https://www.boost.org/doc/libs/1_82_0/libs/beast/example/websocket/server/async/websocket_server_async.cpp
void
fail(beast::error_code ec, char const* what)
{
  std::cerr << what << ": " << ec.message() << "\n";
}

class session : public std::enable_shared_from_this<session>
{
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;

public:
    explicit
    session(tcp::socket&& socket) : ws_(std::move(socket))
    {
    }

    void
    run()
    {
      // We need to be executing within a strand to perform async operations
      // on the I/O objects in this session. Although not strictly necessary
      // for single-threaded contexts, this example code is written to be
      // thread-safe by default.
      net::dispatch(ws_.get_executor(),beast::bind_front_handler(&session::on_run,shared_from_this()));
    }

    void
    on_run()
    {
      ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

      ws_.set_option(websocket::stream_base::decorator(
              [](websocket::response_type& res)
              {
                  res.set(http::field::server,
                          std::string(BOOST_BEAST_VERSION_STRING) +
                          " websocket-server-async");
              }));

      ws_.async_accept(beast::bind_front_handler(&session::on_accept,shared_from_this()));
    }

    void
    on_accept(beast::error_code ec)
    {
      if(ec)
        return fail(ec, "accept");

      do_read();
    }

    void
    do_read()
    {
      ws_.async_read(buffer_,beast::bind_front_handler(&session::on_read,shared_from_this()));
    }

    void
    do_write()
    {
      ws_.async_write(buffer_.data(),beast::bind_front_handler(&session::on_write,shared_from_this()));
    }

    void
    on_read(beast::error_code ec,std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      if(ec == websocket::error::closed)
        return;

      if(ec)
        return fail(ec, "read");

      //ws_.text(ws_.got_text());
      ws_.async_write(buffer_.data(),beast::bind_front_handler(&session::on_write,shared_from_this()));
    }

    void
    on_write(beast::error_code ec,std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);
      if(ec)
        return fail(ec, "write");
      buffer_.consume(buffer_.size());
    }
};

class listener : public std::enable_shared_from_this<listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    listener(net::io_context& ioc,tcp::endpoint endpoint): ioc_(ioc), acceptor_(ioc)
    {
      beast::error_code ec;
      acceptor_.open(endpoint.protocol(), ec);
      if(ec)
      {
        fail(ec, "open");
        return;
      }
      acceptor_.set_option(net::socket_base::reuse_address(true), ec);
      if(ec)
      {
        fail(ec, "set_option");
        return;
      }
      acceptor_.bind(endpoint, ec);
      if(ec)
      {
        fail(ec, "bind");
        return;
      }
      acceptor_.listen(net::socket_base::max_listen_connections, ec);
      if(ec)
      {
        fail(ec, "listen");
        return;
      }
    }

    void
    run()
    {
      do_accept();
    }

private:
    void
    do_accept()
    {
      acceptor_.async_accept(net::make_strand(ioc_),beast::bind_front_handler(&listener::on_accept,shared_from_this()));
    }

    void
    on_accept(beast::error_code ec, tcp::socket socket)
    {
      if(ec)
      {
        fail(ec, "accept");
      }
      else
      {
        std::make_shared<session>(std::move(socket))->run();
      }

      do_accept();
    }
};

int main(int , char**)
{
  auto const address = net::ip::make_address("127.0.0.1");
  auto const port = static_cast<unsigned short>(9090);
  net::io_context ioc{1};
  std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();
  ioc.run();
  return EXIT_SUCCESS;
}
