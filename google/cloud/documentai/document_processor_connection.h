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
// source: google/cloud/documentai/v1/document_processor_service.proto

#ifndef GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_DOCUMENTAI_DOCUMENT_PROCESSOR_CONNECTION_H
#define GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_DOCUMENTAI_DOCUMENT_PROCESSOR_CONNECTION_H

#include "google/cloud/documentai/document_processor_connection_idempotency_policy.h"
#include "google/cloud/documentai/internal/document_processor_retry_traits.h"
#include "google/cloud/documentai/internal/document_processor_stub.h"
#include "google/cloud/backoff_policy.h"
#include "google/cloud/future.h"
#include "google/cloud/options.h"
#include "google/cloud/polling_policy.h"
#include "google/cloud/status_or.h"
#include "google/cloud/stream_range.h"
#include "google/cloud/version.h"
#include <google/longrunning/operations.grpc.pb.h>
#include <memory>
#include <string>

namespace google {
namespace cloud {
namespace documentai {
GOOGLE_CLOUD_CPP_INLINE_NAMESPACE_BEGIN

using DocumentProcessorServiceRetryPolicy =
    ::google::cloud::internal::TraitBasedRetryPolicy<
        documentai_internal::DocumentProcessorServiceRetryTraits>;

using DocumentProcessorServiceLimitedTimeRetryPolicy =
    ::google::cloud::internal::LimitedTimeRetryPolicy<
        documentai_internal::DocumentProcessorServiceRetryTraits>;

using DocumentProcessorServiceLimitedErrorCountRetryPolicy =
    ::google::cloud::internal::LimitedErrorCountRetryPolicy<
        documentai_internal::DocumentProcessorServiceRetryTraits>;

/**
 * The `DocumentProcessorServiceConnection` object for
 * `DocumentProcessorServiceClient`.
 *
 * This interface defines virtual methods for each of the user-facing overload
 * sets in `DocumentProcessorServiceClient`. This allows users to inject custom
 * behavior (e.g., with a Google Mock object) when writing tests that use
 * objects of type `DocumentProcessorServiceClient`.
 *
 * To create a concrete instance, see
 * `MakeDocumentProcessorServiceConnection()`.
 *
 * For mocking, see `documentai_mocks::MockDocumentProcessorServiceConnection`.
 */
class DocumentProcessorServiceConnection {
 public:
  virtual ~DocumentProcessorServiceConnection() = 0;

  virtual Options options() { return Options{}; }

  virtual StatusOr<google::cloud::documentai::v1::ProcessResponse>
  ProcessDocument(google::cloud::documentai::v1::ProcessRequest const& request);

  virtual future<StatusOr<google::cloud::documentai::v1::BatchProcessResponse>>
  BatchProcessDocuments(
      google::cloud::documentai::v1::BatchProcessRequest const& request);

  virtual StatusOr<google::cloud::documentai::v1::FetchProcessorTypesResponse>
  FetchProcessorTypes(
      google::cloud::documentai::v1::FetchProcessorTypesRequest const& request);

  virtual StreamRange<google::cloud::documentai::v1::ProcessorType>
  ListProcessorTypes(
      google::cloud::documentai::v1::ListProcessorTypesRequest request);

  virtual StreamRange<google::cloud::documentai::v1::Processor> ListProcessors(
      google::cloud::documentai::v1::ListProcessorsRequest request);

  virtual StatusOr<google::cloud::documentai::v1::Processor> GetProcessor(
      google::cloud::documentai::v1::GetProcessorRequest const& request);

  virtual StatusOr<google::cloud::documentai::v1::ProcessorVersion>
  GetProcessorVersion(
      google::cloud::documentai::v1::GetProcessorVersionRequest const& request);

  virtual StreamRange<google::cloud::documentai::v1::ProcessorVersion>
  ListProcessorVersions(
      google::cloud::documentai::v1::ListProcessorVersionsRequest request);

  virtual future<
      StatusOr<google::cloud::documentai::v1::DeleteProcessorVersionMetadata>>
  DeleteProcessorVersion(
      google::cloud::documentai::v1::DeleteProcessorVersionRequest const&
          request);

  virtual future<
      StatusOr<google::cloud::documentai::v1::DeployProcessorVersionResponse>>
  DeployProcessorVersion(
      google::cloud::documentai::v1::DeployProcessorVersionRequest const&
          request);

  virtual future<
      StatusOr<google::cloud::documentai::v1::UndeployProcessorVersionResponse>>
  UndeployProcessorVersion(
      google::cloud::documentai::v1::UndeployProcessorVersionRequest const&
          request);

  virtual StatusOr<google::cloud::documentai::v1::Processor> CreateProcessor(
      google::cloud::documentai::v1::CreateProcessorRequest const& request);

  virtual future<
      StatusOr<google::cloud::documentai::v1::DeleteProcessorMetadata>>
  DeleteProcessor(
      google::cloud::documentai::v1::DeleteProcessorRequest const& request);

  virtual future<
      StatusOr<google::cloud::documentai::v1::EnableProcessorResponse>>
  EnableProcessor(
      google::cloud::documentai::v1::EnableProcessorRequest const& request);

  virtual future<
      StatusOr<google::cloud::documentai::v1::DisableProcessorResponse>>
  DisableProcessor(
      google::cloud::documentai::v1::DisableProcessorRequest const& request);

  virtual future<StatusOr<
      google::cloud::documentai::v1::SetDefaultProcessorVersionResponse>>
  SetDefaultProcessorVersion(
      google::cloud::documentai::v1::SetDefaultProcessorVersionRequest const&
          request);

  virtual future<
      StatusOr<google::cloud::documentai::v1::ReviewDocumentResponse>>
  ReviewDocument(
      google::cloud::documentai::v1::ReviewDocumentRequest const& request);
};

/**
 * A factory function to construct an object of type
 * `DocumentProcessorServiceConnection`.
 *
 * The returned connection object should not be used directly; instead it
 * should be passed as an argument to the constructor of
 * DocumentProcessorServiceClient.
 *
 * The optional @p options argument may be used to configure aspects of the
 * returned `DocumentProcessorServiceConnection`. Expected options are any of
 * the types in the following option lists:
 *
 * - `google::cloud::CommonOptionList`
 * - `google::cloud::GrpcOptionList`
 * - `google::cloud::UnifiedCredentialsOptionList`
 * - `google::cloud::documentai::DocumentProcessorServicePolicyOptionList`
 *
 * @note Unexpected options will be ignored. To log unexpected options instead,
 *     set `GOOGLE_CLOUD_CPP_ENABLE_CLOG=yes` in the environment.
 *
 * @param location Sets the prefix for the default `EndpointOption` value.
 * @param options (optional) Configure the `DocumentProcessorServiceConnection`
 * created by this function.
 */
std::shared_ptr<DocumentProcessorServiceConnection>
MakeDocumentProcessorServiceConnection(std::string const& location,
                                       Options options = {});

/**
 * A backwards-compatible version of the previous factory function.  Unless
 * the service also offers a global endpoint, the default value of the
 * `EndpointOption` may be useless, in which case it must be overridden.
 *
 * @deprecated Please use the `location` overload instead.
 */
std::shared_ptr<DocumentProcessorServiceConnection>
MakeDocumentProcessorServiceConnection(Options options = {});

GOOGLE_CLOUD_CPP_INLINE_NAMESPACE_END
}  // namespace documentai
}  // namespace cloud
}  // namespace google

#endif  // GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_DOCUMENTAI_DOCUMENT_PROCESSOR_CONNECTION_H
