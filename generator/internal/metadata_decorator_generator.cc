// Copyright 2020 Google LLC
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

#include "generator/internal/metadata_decorator_generator.h"
#include "absl/memory/memory.h"
#include "absl/strings/str_split.h"
#include "generator/internal/codegen_utils.h"
#include "generator/internal/predicate_utils.h"
#include "generator/internal/printer.h"
#include <google/protobuf/descriptor.h>

namespace google {
namespace cloud {
namespace generator_internal {

MetadataDecoratorGenerator::MetadataDecoratorGenerator(
    google::protobuf::ServiceDescriptor const* service_descriptor,
    VarsDictionary service_vars,
    std::map<std::string, VarsDictionary> service_method_vars,
    google::protobuf::compiler::GeneratorContext* context)
    : ServiceCodeGenerator("metadata_header_path", "metadata_cc_path",
                           service_descriptor, std::move(service_vars),
                           std::move(service_method_vars), context) {}

Status MetadataDecoratorGenerator::GenerateHeader() {
  HeaderPrint(CopyrightLicenseFileHeader());
  HeaderPrint(  // clang-format off
    "\n"
    "// Generated by the Codegen C++ plugin.\n"
    "// If you make any local changes, they will be lost.\n"
    "// source: $proto_file_name$\n"
    "\n"
    "#ifndef $header_include_guard$\n"
    "#define $header_include_guard$\n");
  // clang-format on

  // includes
  HeaderPrint("\n");
  HeaderLocalIncludes({vars("stub_header_path"), "google/cloud/version.h"});
  HeaderSystemIncludes(
      {HasLongrunningMethod() ? "google/longrunning/operations.grpc.pb.h" : "",
       "memory", "string"});

  auto result = HeaderOpenNamespaces(NamespaceType::kInternal);
  if (!result.ok()) return result;

  // metadata decorator class
  HeaderPrint(  // clang-format off
    "\n"
    "class $metadata_class_name$ : public $stub_class_name$ {\n"
    " public:\n"
    "  ~$metadata_class_name$() override = default;\n"
    "  explicit $metadata_class_name$(std::shared_ptr<$stub_class_name$> child);\n");
  // clang-format on

  for (auto const& method : methods()) {
    if (IsStreamingWrite(method)) {
      HeaderPrintMethod(method, __FILE__, __LINE__,
                        R"""(
  std::unique_ptr<::google::cloud::internal::StreamingWriteRpc<
      $request_type$,
      $response_type$>>
  $method_name$(
      std::unique_ptr<grpc::ClientContext> context) override;
)""");
      continue;
    }
    if (IsBidirStreaming(method)) {
      HeaderPrintMethod(method, __FILE__, __LINE__,
                        R"""(
  std::unique_ptr<::google::cloud::AsyncStreamingReadWriteRpc<
      $request_type$,
      $response_type$>>
  Async$method_name$(
      google::cloud::CompletionQueue const& cq,
      std::unique_ptr<grpc::ClientContext> context) override;
)""");
      continue;
    }
    HeaderPrintMethod(
        method,
        {MethodPattern({{IsResponseTypeEmpty,
                         // clang-format off
    "\n  Status $method_name$(\n",
    "\n  StatusOr<$response_type$> $method_name$(\n"},
   {"    grpc::ClientContext& context,\n"
    "    $request_type$ const& request) override;\n"
                            // clang-format on
                        }},
                       And(IsNonStreaming, Not(IsLongrunningOperation))),
         MethodPattern({{R"""(
  future<StatusOr<google::longrunning::Operation>> Async$method_name$(
      google::cloud::CompletionQueue& cq,
      std::unique_ptr<grpc::ClientContext> context,
      $request_type$ const& request) override;
)"""}},
                       IsLongrunningOperation),
         MethodPattern(
             {    // clang-format off
   {"\n"
    "  std::unique_ptr<google::cloud::internal::StreamingReadRpc<$response_type$>>\n"
    "    $method_name$(\n"
    "    std::unique_ptr<grpc::ClientContext> context,\n"
    "    $request_type$ const& request) override;\n"
                  // clang-format on
              }},
             IsStreamingRead)},
        __FILE__, __LINE__);
  }

  for (auto const& method : async_methods()) {
    if (IsStreamingRead(method)) {
      auto constexpr kDeclaration = R"""(
  std::unique_ptr<::google::cloud::internal::AsyncStreamingReadRpc<
      $response_type$>>
  Async$method_name$(
      google::cloud::CompletionQueue const& cq,
      std::unique_ptr<grpc::ClientContext> context,
      $request_type$ const& request) override;
)""";
      HeaderPrintMethod(method, __FILE__, __LINE__, kDeclaration);
      continue;
    }
    HeaderPrintMethod(
        method,
        {MethodPattern(
            {{IsResponseTypeEmpty,
              // clang-format off
    "\n  future<Status> Async$method_name$(\n",
    "\n  future<StatusOr<$response_type$>> Async$method_name$(\n"},
   {"    google::cloud::CompletionQueue& cq,\n"
    "    std::unique_ptr<grpc::ClientContext> context,\n"
    "    $request_type$ const& request) override;\n"
                 // clang-format on
             }},
            And(IsNonStreaming, Not(IsLongrunningOperation)))},
        __FILE__, __LINE__);
  }

  if (HasLongrunningMethod()) {
    HeaderPrint(
        R"""(
  future<StatusOr<google::longrunning::Operation>> AsyncGetOperation(
      google::cloud::CompletionQueue& cq,
      std::unique_ptr<grpc::ClientContext> context,
      google::longrunning::GetOperationRequest const& request) override;

  future<Status> AsyncCancelOperation(
      google::cloud::CompletionQueue& cq,
      std::unique_ptr<grpc::ClientContext> context,
      google::longrunning::CancelOperationRequest const& request) override;
)""");
  }

  HeaderPrint(R"""(
 private:
  void SetMetadata(grpc::ClientContext& context,
                   std::string const& request_params);
  void SetMetadata(grpc::ClientContext& context);

  std::shared_ptr<$stub_class_name$> child_;
  std::string api_client_header_;
};
)""");

  HeaderCloseNamespaces();
  // close header guard
  HeaderPrint("\n#endif  // $header_include_guard$\n");
  return {};
}

Status MetadataDecoratorGenerator::GenerateCc() {
  CcPrint(CopyrightLicenseFileHeader());
  CcPrint(  // clang-format off
            "\n"
            "// Generated by the Codegen C++ plugin.\n"
            "// If you make any local changes, they will be lost.\n"
            "// source: $proto_file_name$\n");
  // clang-format on

  // includes
  CcPrint("\n");
  CcLocalIncludes({vars("metadata_header_path"),
                   "google/cloud/internal/api_client_header.h",
                   "google/cloud/common_options.h",
                   "google/cloud/status_or.h"});
  CcSystemIncludes({vars("proto_grpc_header_path"), "memory"});

  auto result = CcOpenNamespaces(NamespaceType::kInternal);
  if (!result.ok()) return result;

  // constructor
  CcPrint(  // clang-format off
    "\n"
    "$metadata_class_name$::$metadata_class_name$(\n"
    "    std::shared_ptr<$stub_class_name$> child)\n"
    "    : child_(std::move(child)),\n"
    "      api_client_header_(google::cloud::internal::ApiClientHeader(\"generator\")) {}\n");
  // clang-format on

  // metadata decorator class member methods
  for (auto const& method : methods()) {
    if (IsStreamingWrite(method)) {
      CcPrintMethod(method, __FILE__, __LINE__,
                    R"""(
std::unique_ptr<::google::cloud::internal::StreamingWriteRpc<
    $request_type$,
    $response_type$>>
$metadata_class_name$::$method_name$(
    std::unique_ptr<grpc::ClientContext> context) {
  SetMetadata(*context);
  return child_->$method_name$(std::move(context));
}
)""");
      continue;
    }
    if (IsBidirStreaming(method)) {
      CcPrintMethod(method, __FILE__, __LINE__,
                    R"""(
std::unique_ptr<::google::cloud::AsyncStreamingReadWriteRpc<
      $request_type$,
      $response_type$>>
$metadata_class_name$::Async$method_name$(
    google::cloud::CompletionQueue const& cq,
    std::unique_ptr<grpc::ClientContext> context) {
  SetMetadata(*context);
  return child_->Async$method_name$(cq, std::move(context));
}
)""");
      continue;
    }
    CcPrintMethod(
        method,
        {MethodPattern(
             {
                 {IsResponseTypeEmpty,
                  // clang-format off
    "\nStatus\n",
    "\nStatusOr<$response_type$>\n"},
   {"$metadata_class_name$::$method_name$(\n"
    "    grpc::ClientContext& context,\n"
    "    $request_type$ const& request) {\n"},
   {HasRoutingHeader,
    "  SetMetadata(context, \"$method_request_param_key$=\" + request.$method_request_param_value$);\n",
    "  SetMetadata(context, {});\n"},
   {"  return child_->$method_name$(context, request);\n"
    "}\n",}
                 // clang-format on
             },
             And(IsNonStreaming, Not(IsLongrunningOperation))),
         MethodPattern({{HasRoutingHeader,
                         R"""(
future<StatusOr<google::longrunning::Operation>>
$metadata_class_name$::Async$method_name$(
    google::cloud::CompletionQueue& cq,
    std::unique_ptr<grpc::ClientContext> context,
    $request_type$ const& request) {
  SetMetadata(*context, "$method_request_param_key$=" + request.$method_request_param_value$);
  return child_->Async$method_name$(cq, std::move(context), request);
}
)""",
                         R"""(
future<StatusOr<google::longrunning::Operation>>
$metadata_class_name$::Async$method_name$(
    google::cloud::CompletionQueue& cq,
    std::unique_ptr<grpc::ClientContext> context,
    $request_type$ const& request) {
  SetMetadata(*context, {});
  return child_->Async$method_name$(cq, std::move(context), request);
}
)"""}},
                       IsLongrunningOperation),
         MethodPattern(
             {
                 // clang-format off
   {"\n"
    "std::unique_ptr<google::cloud::internal::StreamingReadRpc<$response_type$>>\n"
    "$metadata_class_name$::$method_name$(\n"
    "    std::unique_ptr<grpc::ClientContext> context,\n"
    "    $request_type$ const& request) {\n"},
   {HasRoutingHeader,
    "  SetMetadata(*context, \"$method_request_param_key$=\" + request.$method_request_param_value$);\n",
    "  SetMetadata(*context, {});\n"},
   {"  return child_->$method_name$(std::move(context), request);\n"
    "}\n",}
                 // clang-format on
             },
             IsStreamingRead)},
        __FILE__, __LINE__);
  }

  for (auto const& method : async_methods()) {
    if (IsStreamingRead(method)) {
      auto constexpr kDefinition = R"""(
std::unique_ptr<::google::cloud::internal::AsyncStreamingReadRpc<
      $response_type$>>
$metadata_class_name$::Async$method_name$(
    google::cloud::CompletionQueue const& cq,
    std::unique_ptr<grpc::ClientContext> context,
    $request_type$ const& request) {
  SetMetadata(*context);
  return child_->Async$method_name$(cq, std::move(context), request);
}
)""";
      CcPrintMethod(method, __FILE__, __LINE__, kDefinition);
      continue;
    }
    CcPrintMethod(
        method,
        {MethodPattern(
            {{IsResponseTypeEmpty,
              // clang-format off
    "\nfuture<Status>\n",
    "\nfuture<StatusOr<$response_type$>>\n"},
    {R"""($metadata_class_name$::Async$method_name$(
    google::cloud::CompletionQueue& cq,
    std::unique_ptr<grpc::ClientContext> context,
    $request_type$ const& request) {)"""},
   {HasRoutingHeader, R"""(
  SetMetadata(*context, "$method_request_param_key$=" + request.$method_request_param_value$);)""",
   R"""(
  SetMetadata(*context, {});)"""}, {R"""(
  return child_->Async$method_name$(cq, std::move(context), request);
}
)"""}},
            // clang-format on
            And(IsNonStreaming, Not(IsLongrunningOperation)))},
        __FILE__, __LINE__);
  }

  // long running operation support methods
  if (HasLongrunningMethod()) {
    CcPrint(R"""(
future<StatusOr<google::longrunning::Operation>>
$metadata_class_name$::AsyncGetOperation(
    google::cloud::CompletionQueue& cq,
    std::unique_ptr<grpc::ClientContext> context,
    google::longrunning::GetOperationRequest const& request) {
  SetMetadata(*context, "name=" + request.name());
  return child_->AsyncGetOperation(cq, std::move(context), request);
}

future<Status> $metadata_class_name$::AsyncCancelOperation(
    google::cloud::CompletionQueue& cq,
    std::unique_ptr<grpc::ClientContext> context,
    google::longrunning::CancelOperationRequest const& request) {
  SetMetadata(*context, "name=" + request.name());
  return child_->AsyncCancelOperation(cq, std::move(context), request);
}
)""");
  }

  CcPrint(R"""(
void $metadata_class_name$::SetMetadata(grpc::ClientContext& context,
                                        std::string const& request_params) {
  context.AddMetadata("x-goog-request-params", request_params);
  SetMetadata(context);
}

void $metadata_class_name$::SetMetadata(grpc::ClientContext& context) {
  context.AddMetadata("x-goog-api-client", api_client_header_);
  auto const& options = internal::CurrentOptions();
  if (options.has<UserProjectOption>()) {
    context.AddMetadata(
        "x-goog-user-project", options.get<UserProjectOption>());
  }
  auto const& authority = options.get<AuthorityOption>();
  if (!authority.empty()) context.set_authority(authority);
}
)""");

  CcCloseNamespaces();
  return {};
}

}  // namespace generator_internal
}  // namespace cloud
}  // namespace google
