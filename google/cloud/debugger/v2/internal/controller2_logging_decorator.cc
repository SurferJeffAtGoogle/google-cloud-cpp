// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Generated by the Codegen C++ plugin.
// If you make any local changes, they will be lost.
// source: google/devtools/clouddebugger/v2/controller.proto

#include "google/cloud/debugger/v2/internal/controller2_logging_decorator.h"
#include "google/cloud/internal/log_wrapper.h"
#include "google/cloud/status_or.h"
#include <google/devtools/clouddebugger/v2/controller.grpc.pb.h>
#include <memory>

namespace google {
namespace cloud {
namespace debugger_v2_internal {
GOOGLE_CLOUD_CPP_INLINE_NAMESPACE_BEGIN

Controller2Logging::Controller2Logging(std::shared_ptr<Controller2Stub> child,
                                       TracingOptions tracing_options,
                                       std::set<std::string> components)
    : child_(std::move(child)),
      tracing_options_(std::move(tracing_options)),
      components_(std::move(components)) {}

StatusOr<google::devtools::clouddebugger::v2::RegisterDebuggeeResponse>
Controller2Logging::RegisterDebuggee(
    grpc::ClientContext& context,
    google::devtools::clouddebugger::v2::RegisterDebuggeeRequest const&
        request) {
  return google::cloud::internal::LogWrapper(
      [this](grpc::ClientContext& context,
             google::devtools::clouddebugger::v2::RegisterDebuggeeRequest const&
                 request) {
        return child_->RegisterDebuggee(context, request);
      },
      context, request, __func__, tracing_options_);
}

StatusOr<google::devtools::clouddebugger::v2::ListActiveBreakpointsResponse>
Controller2Logging::ListActiveBreakpoints(
    grpc::ClientContext& context,
    google::devtools::clouddebugger::v2::ListActiveBreakpointsRequest const&
        request) {
  return google::cloud::internal::LogWrapper(
      [this](grpc::ClientContext& context,
             google::devtools::clouddebugger::v2::
                 ListActiveBreakpointsRequest const& request) {
        return child_->ListActiveBreakpoints(context, request);
      },
      context, request, __func__, tracing_options_);
}

StatusOr<google::devtools::clouddebugger::v2::UpdateActiveBreakpointResponse>
Controller2Logging::UpdateActiveBreakpoint(
    grpc::ClientContext& context,
    google::devtools::clouddebugger::v2::UpdateActiveBreakpointRequest const&
        request) {
  return google::cloud::internal::LogWrapper(
      [this](grpc::ClientContext& context,
             google::devtools::clouddebugger::v2::
                 UpdateActiveBreakpointRequest const& request) {
        return child_->UpdateActiveBreakpoint(context, request);
      },
      context, request, __func__, tracing_options_);
}

GOOGLE_CLOUD_CPP_INLINE_NAMESPACE_END
}  // namespace debugger_v2_internal
}  // namespace cloud
}  // namespace google
