// MIT License
//
// Copyright (c) 2023 Scott Cyphers
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef Z0FTWARE_SIGNATURE_HPP
#define Z0FTWARE_SIGNATURE_HPP

#include <tuple>

// Helpers for disecting the signature of a callable
// Adapted from https://www.godbolt.org/z/ev93dbo1G
template <typename Signature> struct signature;
template <typename Result, typename... Parameters>
struct signature<Result(Parameters...)> {
  using result_type = Result;
  using parameters_type = std::tuple<Parameters...>;
};

template <typename Result, typename Class, typename... Parameters>
struct signature<Result (Class::*)(Parameters...)> {
  using result_type = Result;
  using parameters_type = std::tuple<Parameters...>;
};

template <typename Result, typename Class, typename... Parameters>
struct signature<Result (Class::*)(Parameters...) const> {
  using result_type = Result;
  using parameters_type = std::tuple<Parameters...>;
};

template <typename Function>
concept is_function = std::is_function_v<Function>;

template <typename Function>
concept is_member_function =
    std::is_member_function_pointer_v<std::decay_t<Function>>;

template <typename Function>
concept is_functor =
    std::is_class_v<std::decay_t<Function>> &&
    requires(Function &&t) { &std::decay_t<Function>::operator(); };

template <is_functor Function>
auto parameters(Function &&function) -> typename signature<
    decltype(&std::decay_t<Function>::operator())>::parameters_type;

template <is_functor Function>
auto parameters(const Function &function) -> typename signature<
    decltype(&std::decay_t<Function>::operator())>::parameters_type;

template <is_function Function>
auto parameters(const Function &function) ->
    typename signature<Function>::parameters_type;

template <is_member_function Function>
auto parameters(Function &&function) ->
    typename signature<std::decay_t<Function>>::type;

template <is_member_function Function>
auto parameters(const Function &function) ->
    typename signature<std::decay_t<Function>>::type;

#endif
