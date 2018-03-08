from conans import ConanFile


class ArghConan(ConanFile):
    name = "argh"
    version = "1.2.1"
    url = "https://github.com/adishavit/argh"
    description = "Argh! A minimalist argument handler."
    license = "BSD 3-Clause"
    exports = ["LICENSE"]
    exports_sources = "argh.h"

    def package(self):
        self.copy(pattern="LICENSE", dst="license")
        self.copy(pattern="argh.h", dst="include")
