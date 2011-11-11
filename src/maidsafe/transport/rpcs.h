/* Copyright (c) 2010 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAIDSAFE_TRANSPORT_RPCS_H_
#define MAIDSAFE_TRANSPORT_RPCS_H_

#include <vector>

#ifdef __MSVC__
#  pragma warning(push)
#  pragma warning(disable: 4127 4244 4267)
#endif
#include "maidsafe/transport/transport.pb.h"
#ifdef __MSVC__
#  pragma warning(pop)
#endif

#include "maidsafe/transport/message_handler.h"
#include "maidsafe/transport/transport.h"

namespace arg = std::placeholders;

namespace maidsafe {

namespace transport {

class Transport;
struct Endpoint;
struct TransportDetails;
class RudpTransport;
class Contact;
//class MessageHandlerPtr;
//class TransportPtr;

class Rpcs {
  typedef boost::function<void(int, TransportDetails, RudpTransport)> 
      NatResultFunctor;

 public:
  void NatDetection(const std::vector<Contact> &candidates,
                    bool full,
                    NatResultFunctor nrf);
  void NatDetection(const std::vector<Contact> &candidates,
                    TransportPtr listening_transport,
                    bool full,
                    NatResultFunctor nrf);
  void DoNatDetection(const std::vector<Contact> &candidates,
                      TransportPtr transport,
                      MessageHandlerPtr message_handler,
                      const std::string &request,
                      const bool &full,
                      NatResultFunctor nat_result_functor,
                      const size_t &contact_index);

 private:
  void NatDetectionCallback(const TransportCondition &result,
                            const protobuf::NatDetectionResponse &response,
                            const std::vector<Contact> &candidates,
                            NatResultFunctor callback,
                            TransportPtr transport,
                            MessageHandlerPtr message_handler,
                            const std::string &request,
                            const bool &full,
                            const int &contact_index,
                            int index);
};

}  // namespace transport

}  // namespace maidsafe

#endif  // MAIDSAFE_TRANSPORT_RPCS_H_