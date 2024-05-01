// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include "caf/net/dsl/server_factory_base.hpp"
#include "caf/net/fwd.hpp"
#include "caf/net/web_socket/acceptor.hpp"
#include "caf/net/web_socket/server_factory.hpp"

#include "caf/detail/type_list.hpp"
#include "caf/detail/type_traits.hpp"
#include "caf/intrusive_ptr.hpp"

#include <type_traits>

namespace caf::net::web_socket {

/// DSL entry point for creating a server.
template <class Trait>
class has_on_request : public dsl::server_factory_base<has_on_request<Trait>> {
public:
  template <class Token, class... Args>
  has_on_request(Token token, const dsl::generic_config_value& from,
                 Args&&... args) {
    config_ = make_counted<server_config<Trait>>(from.mpx);
    config_->assign(from, token, std::forward<Args>(args)...);
  }

  /// Adds the handler for accepting or rejecting incoming requests.
  template <class OnRequest>
  auto on_request(OnRequest on_request) {
    // Type checking.
    using fn_trait = detail::get_callable_trait_t<OnRequest>;
    static_assert(fn_trait::num_args == 1,
                  "on_request must take exactly one argument");
    using arg_types = typename fn_trait::arg_types;
    using arg1_t = detail::tl_at_t<arg_types, 0>;
    using acceptor_t = std::decay_t<arg1_t>;
    static_assert(is_acceptor_v<acceptor_t>,
                  "on_request must take an acceptor as its argument");
    static_assert(std::is_same_v<arg1_t, acceptor_t&>,
                  "on_request must take the acceptor as mutable reference");
    // Wrap the callback and return the factory object.
    using factory_t = typename acceptor_t::template server_factory_type<Trait>;
    auto callback = make_shared_type_erased_callback(std::move(on_request));
    return factory_t{std::move(config_), std::move(callback)};
  }

protected:
  dsl::server_config_value& base_config() override {
    return *config_;
  }

private:
  intrusive_ptr<server_config<Trait>> config_;
};

} // namespace caf::net::web_socket
