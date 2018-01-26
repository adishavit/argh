from conans import ConanFile
import os


class ArghConan(ConanFile):
    name = "argh"
    version = "1.2.0"
    url = "https://github.com/adishavit/argh"
    description = "Argh! A minimalist argument handler."
    license = "BSD 3-Clause"
    exports = ["LICENSE"]
    exports_sources = "*.h"

    def package(self):
        self.copy(pattern="LICENSE", dst="license")
        self.copy(pattern="argh.h", dst="include")
