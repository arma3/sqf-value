#pragma once

#include "value.hpp"
#include <vector>
#include <optional>
#include <variant>
#include <functional>

namespace sqf
{
    namespace meta
    {
        template <typename ArgType>
        struct is_optional : std::false_type {};
        template <typename T>
        struct is_optional<std::optional<T>> : std::true_type {};
        template <typename ArgType>
        inline constexpr bool is_optional_v = is_optional<ArgType>::value;

        template <typename ArgType>
        struct def_value { static ArgType value() { return {}; } };

        template <typename ArgType>
        struct get_type { using type = ArgType; };
        template <typename ArgType>
        struct get_type<std::optional<ArgType>> { using type = ArgType; };
    }

    struct method {
    public:
        template<typename TPass, typename TErr>
        class ret
        {
            std::optional<TPass> m_passed;
            std::optional<TErr> m_error;
        public:
            ret(std::optional<TPass> p, std::optional<TErr> e) : m_passed(p), m_error(e) {}
            ret(TPass p) : m_passed(p), m_error({}) {}
            bool is_err() const { return m_error.has_value(); }
            bool is_ok() const { return m_passed.has_value(); }
            TErr get_err() const { return m_error.value(); }
            TPass get_ok() const { return m_passed.value(); }
            static ret err(TErr e) { return { {}, e }; }
            static ret ok(TPass p) { return { p, {} }; }
        };
    private:
        std::function<bool(const std::vector<value>&)> m_can_call;
        std::function<ret<value, value>(const std::vector<value>&)> m_call;

        template <typename ... Args, std::size_t... IndexSequence>
        static bool can_call_impl(const std::vector<value>& values, std::index_sequence<IndexSequence...> s) {
            // values max args
            return values.size() <= sizeof...(Args) && 
                // for every Arg, either...
                (... && (
                // the value provides that argument and its the correct type, or...
                (IndexSequence < values.size() && sqf::is<typename sqf::meta::get_type<Args>::type>(values[IndexSequence])) ||
                // the value does not provide that argument and the arg is an optional
                (IndexSequence >= values.size() && sqf::meta::is_optional_v<Args>)
                ));
        }

        template <typename Ret, typename ... Args, std::size_t... IndexSequence>
        static ret<value, value> call_impl_ok(std::function<Ret(Args...)> f, const std::vector<value>& values, std::index_sequence<IndexSequence...>) {
            auto res = // call the function with every type in the value set,
                // padding with empty std::optionals otherwise
                std::invoke(f,
                    (IndexSequence < values.size() ? sqf::get<typename sqf::meta::get_type<Args>::type>(values[IndexSequence])
                        : sqf::meta::def_value<Args>::value())...);
            return ret<value, value>::ok(res);
        }
        template <typename Ret, typename ... Args, std::size_t... IndexSequence>
        static ret<value, value> call_impl(std::function<Ret(Args...)> f, const std::vector<value>& values, std::index_sequence<IndexSequence...>) {
            auto res = // call the function with every type in the value set,
                // padding with empty std::optionals otherwise
                std::invoke(f,
                    (IndexSequence < values.size() ? sqf::get<typename sqf::meta::get_type<Args>::type>(values[IndexSequence])
                        : sqf::meta::def_value<Args>::value())...);
            if (res.is_ok()) { return ret<value, value>::ok(res.get_ok()); }
            return ret<value, value>::err(res.get_err());
        }

    public:
        template <typename Ret, typename ... Args>
        method(std::function<Ret(Args...)> f) :
            m_can_call([](const std::vector<value>& values) -> bool
                {
                    return can_call_impl<Args...>(values, std::index_sequence_for<Args...>{});
                }),
            m_call([f](const std::vector<value>& values) -> ret<value, value>
                {
                    return call_impl_ok<Ret, Args...>(f, values, std::index_sequence_for<Args...>{});
                })
        {
        }
        template <typename RetOk, typename RetErr, typename ... Args>
        method(std::function<ret<RetOk, RetErr>(Args...)> f) :
            m_can_call([](const std::vector<value>& values) -> bool
                {
                    return can_call_impl<Args...>(values, std::index_sequence_for<Args...>{});
                }),
            m_call([f](const std::vector<value>& values) -> ret<value, value>
                {
                    return call_impl<ret<RetOk, RetErr>, Args...>(f, values, std::index_sequence_for<Args...>{});
                })
        {
        }

        bool can_call(const std::vector<value>& values) const { return m_can_call(values); }

        ret<value, value> call_generic(const std::vector<value>& values) const { return m_call(values); }

        // to handle lambda
        template <typename F>
        method static create(F f) { return method{ std::function{f} }; }
    };
} 