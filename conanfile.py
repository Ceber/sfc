import sys
import os
import subprocess
import re
from conans import ConanFile, tools
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake
from conan.tools.layout import cmake_layout


class SfcConan(ConanFile):
    name = "sfc"
    author = "<ERGENSCHAEFFTER, Cédric> <ceber@ergen.fr>"
    url = "https://home.ergen.fr/gitlab/cpp/libs/sfc"
    description = "C++ 'Sequential function chart' 2nd party lib."

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
        "code_coverage": [True, False],
        "build_doc": [True, False],
    }

    default_options = {
        "shared": True,
        "fPIC": True,
        "build_tests": False,
        "code_coverage": False,
        "build_doc": False,
    }

    exports_sources = [
        "src/**",
        "include/**",
        "unit-tests/**",
        "cmake/**",
        "Doxyfile",
        "README.md",
        "LICENSE",
        "CMakeLists.txt",
        "sfcConfig.cmake.in"
    ]

    def set_version(self):
        content = tools.load(os.path.join(self.recipe_folder, "CMakeLists.txt"))

        version_major = (
            re.search(r"[^\)]+VERSION_MAJOR (\d+)[^\)]*\)", content).group(1).strip()
        )
        version_minor = (
            re.search(r"[^\)]+VERSION_MINOR (\d+)[^\)]*\)", content).group(1).strip()
        )
        version_patch = (
            re.search(r"[^\)]+VERSION_PATCH (\d+)[^\)]*\)", content).group(1).strip()
        )

        self.version = "{}.{}.{}".format(version_major, version_minor, version_patch)

    # TODO: Some files like conaninfo.txt, conan.lock
    # should be generated inside the build folder to avoid polluting sources.

    def build_requirements(self):
        if self.options.build_tests:
            self.build_requires(
                "gtest/1.11.0"
            )  # TODO: replace with test_requires in Conan 2.0

    # Defines the build directory among other things.
    def layout(self):
        cmake_layout(self)
        # Override the paths set by cmake_layout and add the architecture to the build folder
        if self.settings.get_safe("arch"):
            self.folders.build = "build/{}-{}-{}-conan-build".format(
                self.settings.os, self.settings.build_type, self.settings.arch
            )
            self.folders.generators = os.path.join(self.folders.build, "conan")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.variables["CONAN_BUILD"] = True
        toolchain.variables["BUILD_TESTS"] = (
            self.options.build_tests == True or self.options.code_coverage == True
        )
        toolchain.variables["BUILD_DOC"] = self.options.build_doc == True
        toolchain.variables["CODE_COVERAGE"] = self.options.code_coverage == True

        # Override BASE_INSTALL_DIR. It is used by CPack.
        toolchain.variables["BASE_INSTALL_DIR"] = self.package_folder

        if tools.cross_building(self) and os.getenv("OECORE_NATIVE_SYSROOT", None):
            toolchain.variables["CONAN_BUILD"] = True
            # Append Yocto toolchain as 'included' in the generated one.
            toolchain_file = "%s%s" % (
                os.getenv("OECORE_NATIVE_SYSROOT"),
                "/usr/share/cmake/OEToolchainConfig.cmake",
            )
            toolchain.blocks["user_toolchain"].values["paths"] = [toolchain_file]

        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        # These target are not supported when cross building to arm.
        if not tools.cross_building(self):
            if self.options.build_doc == "True":
                cmake.build(target="doc")
            if self.options.code_coverage == "True":
                cmake.parallel = False
                cmake.build(target="coverage")  # Run tests + coverage

    def package(self):
        cmake = CMake(self)
        cmake.install()

    # https://docs.conan.io/en/latest/creating_packages/package_information.html
    def package_info(self):
        # These are default values and doesn't need to be adjusted
        self.cpp_info.includedirs = ["include", "public/include"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.builddirs = ["lib/CMake"]
        self.cpp_info.bindirs = ["bin"]

        self.cpp_info.libs = [
            "sfc",
        ]
