/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_RUDP_CONNECTION_MANAGER_H_
#define MAIDSAFE_RUDP_CONNECTION_MANAGER_H_

#include <unordered_map>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <utility>

#include "boost/asio/buffer.hpp"
#include "boost/asio/strand.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/date_time/posix_time/posix_time_duration.hpp"

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class Transport;
class Connection;
class Multiplexer;
class Socket;
class HandshakePacket;

class ConnectionManager {
 public:
  using endpoint      = boost::asio::ip::udp::endpoint;
  using ConnectionPtr = std::shared_ptr<Connection>;
  using Error         = boost::system::error_code;
  using OnConnect     = std::function<void(const Error&, const ConnectionPtr&)>;

 public:
  ConnectionManager(std::shared_ptr<Transport> transport,
                    const boost::asio::io_service::strand& strand,
                    std::shared_ptr<Multiplexer> multiplexer, connection_id this_node_id,
                    asymm::PublicKey this_public_key);
  ~ConnectionManager();

  void Close();

  void Connect(const connection_id& peer_id, const endpoint& peer_endpoint,
               const asymm::PublicKey& peer_public_key,
               std::function<void(const connection_id&, int)> connection_added_functor,
               const boost::posix_time::time_duration& connect_attempt_timeout,
               const boost::posix_time::time_duration& lifespan, OnConnect on_connect,
               const std::function<void()>& failure_functor);

  int AddConnection(std::shared_ptr<Connection> connection);
  bool CloseConnection(const connection_id& peer_id);
  void RemoveConnection(std::shared_ptr<Connection> connection);
  std::shared_ptr<Connection> GetConnection(const connection_id& peer_id);

  void Ping(const connection_id& peer_id, const endpoint& peer_endpoint,
            const std::function<void(int)>& ping_functor);  // NOLINT (Fraser)
  // Returns false if the connection doesn't exist.
  bool Send(const connection_id& peer_id, const std::string& message,
            const std::function<void(int)>& message_sent_functor);  // NOLINT (Fraser)

  // This node's endpoint as viewed by peer
  endpoint ThisEndpoint(const connection_id& peer_id);

  // Called by Transport when bootstrapping a new transport but when we don't create a temporary
  // connection to establish external endpoint (i.e this node's NAT is symmetric)
  void SetBestGuessExternalEndpoint(const endpoint& external_endpoint);

  // Get the remote endpoint offered for NAT detection by peer.
  endpoint RemoteNatDetectionEndpoint(const connection_id& peer_id);

  // Add a socket. Returns a new unique id for the socket.
  uint32_t AddSocket(Socket* socket);
  void RemoveSocket(uint32_t id);
  // Called by the Dispatcher when a new packet arrives for a socket.  Can return nullptr if no
  // appropriate socket found.
  Socket* GetSocket(const boost::asio::const_buffer& data,
                    const endpoint& endpoint);

  size_t NormalConnectionsCount() const;

  connection_id node_id() const;
  const asymm::PublicKey& public_key() const;

  std::string DebugString();

 private:
  ConnectionManager(const ConnectionManager&);
  ConnectionManager& operator=(const ConnectionManager&);

  bool CanStartConnectingTo(connection_id, endpoint) const;
  void MarkDoneConnecting(connection_id peer_id, endpoint peer_ep);

 private:
  typedef std::shared_ptr<Multiplexer> MultiplexerPtr;
  typedef std::set<ConnectionPtr> ConnectionGroup;
  // Map of destination socket id to corresponding socket object.
  typedef std::unordered_map<uint32_t, Socket*> SocketMap;

  void HandlePingFrom(const HandshakePacket& handshake_packet, const endpoint& endpoint);
  ConnectionGroup::iterator FindConnection(const connection_id& peer_id) const;

  // TODO(PeterJ): Instead of using this set, it would be nicer if we
  // added a "not yet connected connection" into the connetions_ group
  // right a way (before it is connected).
  std::set<std::pair<connection_id, endpoint>> being_connected_;

  // Because the connections can be in an idle state with no pending async operations, they are kept
  // alive with a shared_ptr in this set, as well as in the async operation handlers.
  ConnectionGroup connections_;
  mutable std::mutex mutex_;
  std::weak_ptr<Transport> transport_;
  boost::asio::io_service::strand strand_;
  std::shared_ptr<Multiplexer> multiplexer_;
  const connection_id kThisNodeId_;
  asymm::PublicKey this_public_key_;
  SocketMap sockets_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CONNECTION_MANAGER_H_
