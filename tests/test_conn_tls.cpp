/* Copyright (c) 2018-2022 Marcelo Zimbres Silva (mzimbres@gmail.com)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE.txt)
 */

#include <boost/redis/connection.hpp>
#define BOOST_TEST_MODULE conn-tls
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include "common.hpp"

namespace net = boost::asio;

using connection = boost::redis::connection;
using boost::redis::request;
using boost::redis::response;
using boost::redis::config;
using boost::redis::operation;

bool verify_certificate(bool, net::ssl::verify_context&)
{
   std::cout << "set_verify_callback" << std::endl;
   return true;
}

BOOST_AUTO_TEST_CASE(ping)
{
   config cfg;
   cfg.use_ssl = true;
   cfg.username = "aedis";
   cfg.password = "aedis";
   cfg.addr.host = "db.occase.de";
   cfg.addr.port = "6380";

   std::string const in = "Kabuf";

   request req;
   req.push("PING", in);

   response<std::string> resp;

   net::io_context ioc;
   connection conn{ioc};
   conn.next_layer().set_verify_mode(net::ssl::verify_peer);
   conn.next_layer().set_verify_callback(verify_certificate);

   conn.async_exec(req, resp, [&](auto ec, auto) {
      BOOST_TEST(!ec);
      conn.cancel();
   });

   conn.async_run(cfg, {}, [](auto) { });

   ioc.run();

   BOOST_CHECK_EQUAL(in, std::get<0>(resp).value());
   std::cout << "===============================" << std::endl;
}

