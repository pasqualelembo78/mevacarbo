// Copyright (c) 2012-2016, The MevaCoin developers, The Bytecoin developers
//
// This file is part of Karbo.
//
// Karbo is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Karbo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Karbo.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <string>

namespace Common {

std::string NativePathToGeneric(const std::string& nativePath);

std::string GetPathDirectory(const std::string& path);
std::string GetPathFilename(const std::string& path);
void SplitPath(const std::string& path, std::string& directory, std::string& filename);

std::string CombinePath(const std::string& path1, const std::string& path2);
std::string GetExtension(const std::string& path);
std::string RemoveExtension(const std::string& path);
std::string ReplaceExtenstion(const std::string& path, const std::string& extension);
bool HasParentPath(const std::string& path);
bool validateCertPath(std::string& path);

}
