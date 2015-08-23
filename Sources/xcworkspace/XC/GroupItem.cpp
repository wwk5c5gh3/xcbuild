// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcworkspace/XC/GroupItem.h>
#include <xcworkspace/XC/Workspace.h>

using xcworkspace::XC::GroupItem;

GroupItem::GroupItem(Type type) :
    _type(type)
{
}

std::string GroupItem::
resolve(std::shared_ptr<Workspace> const &workspace) const
{
    std::string location = _location.empty() ? "" : "/" + _location;

    if (_locationType == "container") {
        return workspace->basePath() + location;
    } else if (_locationType == "self") {
        // TODO(grp): Verify this is correct.
        return workspace->basePath() + "/.." + location;
    } else if (_locationType == "group") {
        if (_parent != nullptr) {
            return _parent->resolve(workspace) + location;
        } else {
            return workspace->basePath() + location;
        }
    } else if (_locationType == "absolute") {
        return location;
    } else {
        fprintf(stderr, "error: unknown container type %s\n", _locationType.c_str());
        return location;
    }
}

bool GroupItem::
parse(plist::Dictionary const *dict)
{
    auto L = dict->value <plist::String> ("location");

    if (L != nullptr) {
        std::string location = L->value();
        size_t colon = location.find(':');
        if (colon != std::string::npos) {
            _location     = location.substr(colon + 1);
            _locationType = location.substr(0, colon);
        }
    }

    return true;
}