// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>
#include <pbxbuild/pbxbuild.h>

int
main(int argc, char **argv)
{
    std::unique_ptr<pbxbuild::BuildEnvironment> buildEnvironment = pbxbuild::BuildEnvironment::Default();
    if (buildEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    if (argc < 2) {
        fprintf(stderr, "usage: %s project\n", argv[0]);
        return -1;
    }

    auto project = pbxproj::PBX::Project::Open(argv[1]);
    if (!project) {
        fprintf(stderr, "error opening project at %s (%s)\n", argv[1], strerror(errno));
        return -1;
    }

    printf("Project: %s\n", project->name().c_str());
    auto target = project->targets().front();
    printf("Target: %s\n", target->name().c_str());

    pbxbuild::WorkspaceContext workspaceContext = pbxbuild::WorkspaceContext::Project(project);
    pbxbuild::BuildContext context = pbxbuild::BuildContext::Create(
        workspaceContext,
        nullptr,
        "build",
        "Release"
    );

    std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironment = context.targetEnvironment(*buildEnvironment, target);
    if (targetEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't compute environment\n");
        return -1;
    }

    pbxsetting::Condition condition = pbxsetting::Condition({
        // { "sdk", sdk->canonicalName() },
        { "arch", targetEnvironment->environment().resolve("CURRENT_ARCH") },
        { "variant", targetEnvironment->environment().resolve("CURRENT_VARIANT") },
    });

    std::unordered_map<std::string, std::string> values = targetEnvironment->environment().computeValues(condition);
    std::map<std::string, std::string> orderedValues = std::map<std::string, std::string>(values.begin(), values.end());

    printf("\n\nBuild Settings:\n\n");
    for (auto const &value : orderedValues) {
        printf("    %s = %s\n", value.first.c_str(), value.second.c_str());
    }

    return 0;
}
