# Cloud Resource Manager API C++ Client Library

This directory contains an idiomatic C++ client library for
[Resource Manager][cloud-product], a GCP product to hierarchically manage
resources by project, folder, and organization.

While this library is **GA**, please note that the Google Cloud C++ client libraries do **not** follow
[Semantic Versioning](https://semver.org/).

## Quickstart

The [quickstart/](quickstart/README.md) directory contains a minimal environment
to get started using this client library in a larger project. The following
"Hello World" program is used in this quickstart, and should give you a taste of
this library.

For detailed instructions on how to build and install this library, see the
top-level [README](/README.md#building-and-installing).

<!-- inject-quickstart-start -->

```cc
#include "google/cloud/resourcemanager/v3/projects_client.h"
#include <iostream>

int main(int argc, char* argv[]) try {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " folder-id\n";
    return 1;
  }

  namespace resourcemanager = ::google::cloud::resourcemanager_v3;
  auto client = resourcemanager::ProjectsClient(
      resourcemanager::MakeProjectsConnection());

  for (auto p : client.ListProjects("folders/" + std::string(argv[1]))) {
    if (!p) throw std::move(p).status();
    std::cout << p->DebugString() << "\n";
  }

  return 0;
} catch (google::cloud::Status const& status) {
  std::cerr << "google::cloud::Status thrown: " << status << "\n";
  return 1;
}
```

<!-- inject-quickstart-end -->

## More Information

- Official documentation about the [Cloud Resource Manager][cloud-service-docs] service
- [Reference doxygen documentation][doxygen-link] for each release of this
  client library
- Detailed header comments in our [public `.h`][source-link] files

[cloud-product]: https://cloud.google.com/resource-manager
[cloud-service-docs]: https://cloud.google.com/resource-manager/docs
[doxygen-link]: https://googleapis.dev/cpp/google-cloud-resourcemanager/latest/
[source-link]: https://github.com/googleapis/google-cloud-cpp/tree/main/google/cloud/resourcemanager
