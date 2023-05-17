// Copyright (c) 2023 UltiMaker
// CuraEngine is released under the terms of the AGPLv3 or higher

#ifndef CURAENGINE_INCLUDE_PLUGINS_SLOTS_H
#define CURAENGINE_INCLUDE_PLUGINS_SLOTS_H

#include <memory>
#include <range/v3/range/primitives.hpp>
#include <unordered_map>
#include <variant>

#include "plugins/slotproxy.h"
#include "plugins/types.h"
#include "plugins/validator.h"
#include "plugins/converters.h"

#include "plugin.grpc.pb.h"
#include "plugin.pb.h"

namespace cura::plugins
{

using simplify_slot = SlotProxy<SlotID::SIMPLIFY,
                                Validator<"<=0.0.1", "qwerty-azerty-temp-hash">,
                                proto::Simplify::Stub,
                                agrpc::RPC<&proto::Simplify::Stub::PrepareAsyncSimplify>,
                                simplify_request,
                                simplify_response>;
using postprocess_slot = SlotProxy<SlotID::POSTPROCESS,
                                   Validator<">=1.0.0 <2.0.0 || >3.2.1", "qwerty-azerty-temp-hash">,
                                   proto::Postprocess::Stub,
                                   agrpc::RPC<&proto::Postprocess::Stub::PrepareAsyncPostprocess>,
                                   postprocess_request,
                                   postprocess_response>;

using slots_t = std::variant<simplify_slot, postprocess_slot>;


class Slots
{
    constexpr Slots() noexcept = default;
    std::unordered_map<SlotID, slots_t> slots_{};

public:
    Slots(const Slots&) = delete;
    Slots(Slots&&) = delete;

    static Slots& instance() noexcept
    {
        static Slots instance{};
        return instance;
    }

    template<class T>
    constexpr void set(T&& plugin)
    {
        slots_.emplace(T::slot_id, std::forward<T>(plugin));
    }

    template<class T>
    constexpr auto get() const
    {
        return std::get<T>(slots_.at(T::slot_id));
    }
};
} // namespace cura::plugins

#endif // CURAENGINE_INCLUDE_PLUGINS_SLOTS_H
