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

#include "generator/internal/connection_generator.h"
#include "absl/memory/memory.h"
#include "generator/internal/codegen_utils.h"
#include "generator/internal/descriptor_utils.h"
#include "generator/internal/predicate_utils.h"
#include "generator/internal/printer.h"
#include <google/protobuf/descriptor.h>

namespace google {
namespace cloud {
namespace generator_internal {

ConnectionGenerator::ConnectionGenerator(
    google::protobuf::ServiceDescriptor const* service_descriptor,
    VarsDictionary service_vars,
    std::map<std::string, VarsDictionary> service_method_vars,
    google::protobuf::compiler::GeneratorContext* context)
    : ServiceCodeGenerator("connection_header_path", "connection_cc_path",
                           service_descriptor, std::move(service_vars),
                           std::move(service_method_vars), context) {}

Status ConnectionGenerator::GenerateHeader() {
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
  HeaderLocalIncludes(
      {vars("idempotency_policy_header_path"), vars("stub_header_path"),
       vars("retry_traits_header_path"), "google/cloud/backoff_policy.h",
       HasLongrunningMethod() ? "google/cloud/future.h" : "",
       "google/cloud/options.h",
       HasLongrunningMethod() ? "google/cloud/polling_policy.h" : "",
       "google/cloud/status_or.h",
       HasStreamingReadMethod() || HasPaginatedMethod()
           ? "google/cloud/stream_range.h"
           : "",
       HasBidirStreamingMethod()
           ? "google/cloud/internal/async_read_write_stream_impl.h"
           : "",
       HasBidirStreamingMethod() ? "google/cloud/experimental_tag.h" : "",
       "google/cloud/version.h"});
  HeaderSystemIncludes(
      {HasLongrunningMethod() ? "google/longrunning/operations.grpc.pb.h" : "",
       "memory"});

  auto result = HeaderOpenNamespaces();
  if (!result.ok()) return result;

  HeaderPrint(  // clang-format off
    "\n"
    "using $retry_policy_name$ = ::google::cloud::internal::TraitBasedRetryPolicy<\n"
    "    $product_internal_namespace$::$retry_traits_name$>;\n"
    "\n"
    "using $limited_time_retry_policy_name$ = ::google::cloud::internal::LimitedTimeRetryPolicy<\n"
    "    $product_internal_namespace$::$retry_traits_name$>;\n"
    "\n"
    "using $limited_error_count_retry_policy_name$ =\n"
    "    ::google::cloud::internal::LimitedErrorCountRetryPolicy<\n"
    "        $product_internal_namespace$::$retry_traits_name$>;\n"
                // clang-format on
  );

  // TODO(#8234): This is a special case for backwards compatibility of the
  //     streaming update function.
  if (vars().at("service_name") == "BigQueryRead") {
    // streaming updater functions
    for (auto const& method : methods()) {
      HeaderPrintMethod(
          method,
          {MethodPattern(
              {// clang-format off
     {"\n"
      "GOOGLE_CLOUD_CPP_DEPRECATED(\n"
      "    \"applications should not need this.\"\n"
      "    \" Please file a bug at https://github.com/googleapis/google-cloud-cpp\"\n"
      "    \" if you do.\")"
      "void $service_name$$method_name$StreamingUpdater(\n"
      "    $response_type$ const& response,\n"
      "    $request_type$& request);\n"}
       }, IsStreamingRead)},
               // clang-format on
          __FILE__, __LINE__);
    }
  }

  // Abstract interface Connection base class
  HeaderPrint(  // clang-format off
    "\n"
    "class $connection_class_name$ {\n"
    " public:\n"
    "  virtual ~$connection_class_name$() = 0;\n");
  // clang-format on

  HeaderPrint(R"""(
  virtual Options options() { return Options{}; }
)""");

  for (auto const& method : methods()) {
    if (IsBidirStreaming(method)) {
      HeaderPrintMethod(method, __FILE__, __LINE__,
                        R"""(
  virtual std::unique_ptr<::google::cloud::AsyncStreamingReadWriteRpc<
      $request_type$,
      $response_type$>>
  Async$method_name$(ExperimentalTag);
)""");
      continue;
    }
    HeaderPrintMethod(
        method,
        {MethodPattern(
             {
                 {IsResponseTypeEmpty,
                  // clang-format off
    "\n  virtual Status\n",
    "\n  virtual StatusOr<$response_type$>\n"},
   {"  $method_name$($request_type$ const& request);\n"}
                 // clang-format on
             },
             All(IsNonStreaming, Not(IsLongrunningOperation),
                 Not(IsPaginated))),
         MethodPattern(
             {
                 {IsResponseTypeEmpty,
                  // clang-format off
    "\n  virtual future<Status>\n",
    "\n  virtual future<StatusOr<$longrunning_deduced_response_type$>>\n"},
   {"  $method_name$($request_type$ const& request);\n"}
                 // clang-format on
             },
             All(IsNonStreaming, IsLongrunningOperation, Not(IsPaginated))),
         MethodPattern(
             {
                 // clang-format off
   {"\n  virtual StreamRange<$range_output_type$>\n"
    "  $method_name$($request_type$ request);\n"},
                 // clang-format on
             },
             All(IsNonStreaming, Not(IsLongrunningOperation), IsPaginated)),
         MethodPattern(
             {
                 // clang-format off
   {"\n  virtual StreamRange<$response_type$>\n"
    "  $method_name$($request_type$ const& request);\n"},
                 // clang-format on
             },
             IsStreamingRead)},
        __FILE__, __LINE__);
  }

  for (auto const& method : async_methods()) {
    if (IsStreamingRead(method)) continue;
    HeaderPrintMethod(
        method,
        {MethodPattern(
            {
                {IsResponseTypeEmpty,
                 // clang-format off
    "\n  virtual future<Status>\n",
    "\n  virtual future<StatusOr<$response_type$>>\n"},
   {"  Async$method_name$($request_type$ const& request);\n"}
                // clang-format on
            },
            All(IsNonStreaming, Not(IsLongrunningOperation),
                Not(IsPaginated)))},
        __FILE__, __LINE__);
  }

  // close abstract interface Connection base class
  HeaderPrint("};\n");

  HeaderPrint(  // clang-format off
    "\nstd::shared_ptr<$connection_class_name$> Make$connection_class_name$(\n"
    "    Options options = {});\n");
  // clang-format on

  HeaderCloseNamespaces();

  HeaderOpenNamespaces(NamespaceType::kInternal);
  HeaderPrint(
      // clang-format off
      "\n"
      "std::shared_ptr<$product_namespace$::$connection_class_name$>\n"
      "Make$connection_class_name$(\n"
      "    std::shared_ptr<$stub_class_name$> stub,\n"
      "    Options options);\n");
  // clang-format on
  HeaderCloseNamespaces();

  // close header guard
  HeaderPrint("\n#endif  // $header_include_guard$\n");
  return {};
}

Status ConnectionGenerator::GenerateCc() {
  CcPrint(CopyrightLicenseFileHeader());
  CcPrint(  // clang-format off
    "\n"
    "// Generated by the Codegen C++ plugin.\n"
    "// If you make any local changes, they will be lost.\n"
    "// source: $proto_file_name$\n");
  // clang-format on

  // includes
  CcPrint("\n");
  CcLocalIncludes(
      {vars("connection_header_path"), vars("options_header_path"),
       vars("connection_impl_header_path"), vars("option_defaults_header_path"),
       vars("stub_factory_header_path"), "google/cloud/background_threads.h",
       "google/cloud/common_options.h", "google/cloud/grpc_options.h",
       HasPaginatedMethod() ? "google/cloud/internal/pagination_range.h" : ""});
  CcSystemIncludes({"memory"});

  auto result = CcOpenNamespaces();
  if (!result.ok()) return result;

  if (vars().at("service_name") == "BigQueryRead") {
    CcPrint(R"""(
void BigQueryReadReadRowsStreamingUpdater(
    google::cloud::bigquery::storage::v1::ReadRowsResponse const& response,
    google::cloud::bigquery::storage::v1::ReadRowsRequest& request) {
  return bigquery_internal::BigQueryReadReadRowsStreamingUpdater(response,
                                                                 request);
}
)""");
  }

  CcPrint(R"""(
$connection_class_name$::~$connection_class_name$() = default;
)""");

  for (auto const& method : methods()) {
    if (IsBidirStreaming(method)) {
      CcPrintMethod(method, __FILE__, __LINE__,
                    R"""(
std::unique_ptr<::google::cloud::AsyncStreamingReadWriteRpc<
    $request_type$,
    $response_type$>>
$connection_class_name$::Async$method_name$(ExperimentalTag) {
  return absl::make_unique<
      ::google::cloud::internal::AsyncStreamingReadWriteRpcError<
          $request_type$,
          $response_type$>>(
      Status(StatusCode::kUnimplemented, "not implemented"));
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
   {"$connection_class_name$::$method_name$(\n"
    "    $request_type$ const&) {\n"
    "  return Status(StatusCode::kUnimplemented, \"not implemented\");\n"
    "}\n"
    },
                 // clang-format on
             },
             All(IsNonStreaming, Not(IsLongrunningOperation),
                 Not(IsPaginated))),
         MethodPattern(
             {
                 {IsResponseTypeEmpty,
                  // clang-format off
    "\nfuture<Status>\n",
    "\nfuture<StatusOr<$longrunning_deduced_response_type$>>\n"},
   {"$connection_class_name$::$method_name$(\n"
    "    $request_type$ const&) {\n"
    "  return google::cloud::make_ready_future<\n"
    "    StatusOr<$longrunning_deduced_response_type$>>(\n"
    "    Status(StatusCode::kUnimplemented, \"not implemented\"));\n"
    "}\n"
    },
                 // clang-format on
             },
             All(IsNonStreaming, IsLongrunningOperation, Not(IsPaginated))),
         MethodPattern(
             {
                 // clang-format off
   {"\nStreamRange<$range_output_type$> $connection_class_name$::$method_name$(\n"
    "    $request_type$) {  // NOLINT(performance-unnecessary-value-param)\n"
    "  return google::cloud::internal::MakeUnimplementedPaginationRange<\n"
    "      StreamRange<$range_output_type$>>();\n"
    "}\n"
                     // clang-format on
                 },
             },
             All(IsNonStreaming, Not(IsLongrunningOperation), IsPaginated)),
         MethodPattern(
             {
                 // clang-format off
   {"\nStreamRange<$response_type$> $connection_class_name$::$method_name$(\n"
    "    $request_type$ const&) {\n"
    "  return google::cloud::internal::MakeStreamRange<\n"
    "      $response_type$>(\n"
    "      []() -> absl::variant<Status,\n"
    "      $response_type$>{\n"
    "        return Status(StatusCode::kUnimplemented, \"not implemented\");}\n"
    "      );\n"
    "}\n"
                     // clang-format on
                 },
             },
             IsStreamingRead)},
        __FILE__, __LINE__);
  }

  for (auto const& method : async_methods()) {
    if (IsStreamingRead(method)) continue;
    CcPrintMethod(
        method,
        {MethodPattern({{IsResponseTypeEmpty, R"""(
future<Status>
$connection_class_name$::Async$method_name$(
    $request_type$ const&) {
  return google::cloud::make_ready_future(
      Status(StatusCode::kUnimplemented, "not implemented"));
}
)""",
                         R"""(
future<StatusOr<$response_type$>>
$connection_class_name$::Async$method_name$(
    $request_type$ const&) {
  return google::cloud::make_ready_future<
    StatusOr<$response_type$>>(
    Status(StatusCode::kUnimplemented, "not implemented"));
}
)"""}},
                       All(IsNonStreaming, Not(IsLongrunningOperation),
                           Not(IsPaginated)))},
        __FILE__, __LINE__);
  }

  CcPrint(R"""(
std::shared_ptr<$connection_class_name$> Make$connection_class_name$(
    Options options) {
  internal::CheckExpectedOptions<CommonOptionList, GrpcOptionList,
      $service_name$PolicyOptionList>(options, __func__);
  options = $product_internal_namespace$::$service_name$DefaultOptions(
      std::move(options));
  auto background = internal::MakeBackgroundThreadsFactory(options)();
  auto stub = $product_internal_namespace$::CreateDefault$stub_class_name$(
    background->cq(), options);
  return std::make_shared<$product_internal_namespace$::$connection_class_name$Impl>(
      std::move(background), std::move(stub), std::move(options));
}
)""");

  CcCloseNamespaces();

  CcOpenNamespaces(NamespaceType::kInternal);

  CcPrint(
      R"""(
std::shared_ptr<$product_namespace$::$connection_class_name$>
Make$connection_class_name$(
    std::shared_ptr<$stub_class_name$> stub, Options options) {
  options = $service_name$DefaultOptions(std::move(options));
  auto background = internal::MakeBackgroundThreadsFactory(options)();
  return std::make_shared<$product_internal_namespace$::$connection_class_name$Impl>(
      std::move(background), std::move(stub), std::move(options));
}
)""");

  CcCloseNamespaces();
  return {};
}

}  // namespace generator_internal
}  // namespace cloud
}  // namespace google
