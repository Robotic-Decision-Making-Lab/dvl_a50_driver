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

#pragma once

#include <array>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

#include "dvl_a50_driver/report.hpp"

namespace waterlinked
{

/**
 * @brief Request response from the DVL A50.
 */
struct Response
{
  bool success;               // Whether or not the request was executed successfully
  std::string error_message;  // Error message if the request was not successful
};

class DvlA50Driver
{
public:
  /**
   * @brief Constructor.
   *
   * @param ip The IP address of the DVL A50.
   * @param port The port of the DVL A50, defaults to 16171.
   */
  DvlA50Driver(const std::string & ip, int port = 16171);

  ~DvlA50Driver();

  /**
   * @brief Calibrate the DVL gyroscope.
   *
   * @param timeout The timeout for the calibration response (s)
   *
   * @return Calibration result. If a timeout occurs, the future will contain an empty optional.
   */
  std::future<std::optional<Response>> calibrate_gyro(std::chrono::duration<double> timeout = std::chrono::seconds(3));

  /**
   * @brief Trigger an acoustic ping.
   *
   * @details In setups where multiple acoustic sensors are used, it can be useful to control the pinging of each
   *          acoustic sensor individually. By setting the configuration `acoustic_enabled` = false the pinging of the
   *          DVL can be externally controlled. Up to 15 external trigger commands can be queued by issuing the
   *          trigger_ping command. The DVL will execute each ping in quick succession until no more commands are in the
   *          queue.
   *
   * @param timeout The timeout for the ping response (s)
   *
   * @return Ping request result. If a timeout occurs, the future will contain an empty optional.
   */
  std::future<std::optional<Response>> trigger_ping(std::chrono::duration<double> timeout = std::chrono::seconds(3));

  /**
   * @brief Reset the dead reckoning estimates.
   *
   * @param timeout The timeout for the reset response (s)
   *
   * @return Reset result. If a timeout occurs, the future will contain an empty optional.
   */
  std::future<std::optional<Response>> reset_dead_reckoning(
    std::chrono::duration<double> timeout = std::chrono::seconds(3));

  /**
   * @brief Set the configuration of the DVL.
   *
   * @param config The configuration to set. This should be provided as a JSON string with the top-level key
   *               "parameters". For instance, to set the speed of sound to 1480 m/s, the config string would be
   *               `"parameters": {"speed_of_sound": 1480}`.
   * @param timeout The timeout for the set config response (s)
   */
  std::future<std::optional<Response>> set_config(
    const std::string & config, std::chrono::duration<double> timeout = std::chrono::seconds(3));

  void attach_velocity_callback(std::function<void(const VelocityReport &)> callback);

  void attach_dead_reckoning_callback(std::function<void(const DeadReckoningReport &)> callback);

private:
  struct Request
  {
    std::chrono::time_point<std::chrono::steady_clock> ts;
    std::chrono::duration<double> timeout;
    std::promise<std::optional<Response>> promise;
  };

  void poll();

  void send(const std::string & message);

  void receive(std::string & message);

  std::future<std::optional<Response>> send_request(
    const std::string & type, const std::string & command, std::chrono::duration<double> timeout);

  std::thread poll_thread_;      // Poll incoming data
  std::thread watchdog_thread_;  // Check for request timeouts

  std::unordered_map<std::string, std::queue<Request>> responses_;  // *Evan cackles mischieviously*
  std::mutex responses_mutex_;
};

}  // namespace waterlinked
