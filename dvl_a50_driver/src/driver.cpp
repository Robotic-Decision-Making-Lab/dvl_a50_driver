// Copyright 2024, Evan Palmer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "dvl_a50_driver/driver.hpp"

#include <sstream>

namespace waterlinked
{

DvlA50Driver::DvlA50Driver(const std::string & ip, int port) {}

DvlA50Driver::~DvlA50Driver() {}

std::future<std::optional<Response>> DvlA50Driver::send_request(
  const std::string & type, const std::string & command, std::chrono::duration<double> timeout)
{
  // TODO(evan): send the message

  std::promise<std::optional<Response>> promise;
  std::future<std::optional<Response>> future = promise.get_future();

  const Request request = {std::chrono::steady_clock::now(), timeout, std::move(promise)};

  {
    std::lock_guard<std::mutex> lock(responses_mutex_);
    responses_[type].push(std::move(request));
  }

  return future;
}

std::future<std::optional<Response>> DvlA50Driver::calibrate_gyro(std::chrono::duration<double> timeout)
{
  const std::string command = R"({"command": "calibrate_gyro"})";
  return send_request("calibrate_gyro", command, timeout);
}

std::future<std::optional<Response>> DvlA50Driver::trigger_ping(std::chrono::duration<double> timeout)
{
  const std::string command = R"({"command": "trigger_ping"})";
  return send_request("trigger_ping", command, timeout);
}

std::future<std::optional<Response>> DvlA50Driver::reset_dead_reckoning(std::chrono::duration<double> timeout)
{
  const std::string command = R"({"command": "reset_dead_reckoning"})";
  return send_request("reset_dead_reckoning", command, timeout);
}

std::future<std::optional<Response>> DvlA50Driver::set_config(
  const std::string & config, std::chrono::duration<double> timeout)
{
  std::stringstream ss;
  ss << R"({"command": "set_config",)" << config << "}";
  return send_request("set_config", ss.str(), timeout);
}

}  // namespace waterlinked
