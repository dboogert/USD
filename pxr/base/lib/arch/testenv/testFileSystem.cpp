//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//

#include "pxr/pxr.h"
#include "pxr/base/arch/fileSystem.h"
#include "pxr/base/arch/error.h"
#include "pxr/base/arch/pragmas.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

PXR_NAMESPACE_USING_DIRECTIVE

ARCH_PRAGMA_DEPRECATED_POSIX_NAME

int main()
{
    std::string firstName = ArchMakeTmpFileName("archFS");
    FILE *firstFile;

    char const * const testContent = "text in a file";

    // Open a file, check that its length is 0, write to it, close it, and then
    // check that its length is now the number of characters written.
    ARCH_AXIOM((firstFile = ArchOpenFile(firstName.c_str(), "wb")) != NULL);
    fflush(firstFile);
    ARCH_AXIOM(ArchGetFileLength(firstName.c_str()) == 0);
    fputs(testContent, firstFile);
    fclose(firstFile);
    ARCH_AXIOM(ArchGetFileLength(firstName.c_str()) == strlen(testContent));

    // Map the file and assert the bytes are what we expect they are.
    ARCH_AXIOM((firstFile = ArchOpenFile(firstName.c_str(), "rb")) != NULL);
    ArchConstFileMapping cfm = ArchMapFileReadOnly(firstFile);
    fclose(firstFile);
    ARCH_AXIOM(cfm);
    ARCH_AXIOM(memcmp(testContent, cfm.get(), strlen(testContent)) == 0);
    cfm.reset();

    // Try again with a mutable mapping.
    ARCH_AXIOM((firstFile = ArchOpenFile(firstName.c_str(), "rb")) != NULL);
    ArchMutableFileMapping mfm = ArchMapFileReadWrite(firstFile);
    fclose(firstFile);
    ARCH_AXIOM(mfm);
    ARCH_AXIOM(memcmp(testContent, mfm.get(), strlen(testContent)) == 0);
    // Check that we can successfully mutate.
    mfm.get()[0] = 'T'; mfm.get()[2] = 's';
    ARCH_AXIOM(memcmp("Test", mfm.get(), strlen("Test")) == 0);
    mfm.reset();
    ArchUnlinkFile(firstName.c_str());

    // Test ArchPWrite and ArchPRead.
    int64_t len = strlen(testContent);
    ARCH_AXIOM((firstFile = ArchOpenFile(firstName.c_str(), "w+b")) != NULL);
    ARCH_AXIOM(ArchPWrite(firstFile, testContent, len, 0) == len);
    std::unique_ptr<char[]> buf(new char[len]);
    ARCH_AXIOM(ArchPRead(firstFile, buf.get(), len, 0) == len);
    ARCH_AXIOM(memcmp(testContent, buf.get(), len) == 0);
    char const * const newText = "overwritten in a file";
    ARCH_AXIOM(ArchPWrite(firstFile, newText, strlen(newText),
                      5/*index of 'in a file'*/) == strlen(newText));
    std::unique_ptr<char[]> buf2(new char[strlen("written in a")]);
    ARCH_AXIOM(ArchPRead(firstFile, buf2.get(), strlen("written in a"),
                     9/*index of 'written in a'*/) == strlen("written in a"));
    ARCH_AXIOM(memcmp("written in a", buf2.get(), strlen("written in a")) == 0);

    // create and remove a tmp subdir
    std::string retpath;
    retpath = ArchMakeTmpSubdir(ArchGetTmpDir(), "myprefix");
    ARCH_AXIOM (retpath != "");
    ArchRmDir(retpath.c_str());
    return 0;
}
