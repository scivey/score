# score

score is a c++ library oriented towards web crawling and content extraction tasks in many languages.  The name is not an acronym, has no particular meaning, and can be formatted as `score`, `SCORE` or `Score` according to your own wishes and those of your loved ones.

## Outline

score is backed by a number of established C/C++ libraries.  Its general philosophy is to blatantly steal where possible, adapt and borrow where stealing is infeasible, and resort to custom code in all other cases.  To ease dependency management, it only links to system libraries when those libraries are both mature and commonly available through Linux package managers.  All other dependencies are vendored.

Among other things, score includes the following:
* A high-level C++ HTTP(S) client built around `libCURL`'s high-performance asynchronous interface.
* Character encoding detection and normalization, enabling tasks like "whatever this buffer is encoded as, give me something back in UTF8."  Powered by a vendored `libcharsetdetect` and the system's `libiconv`.
* Language detection powered by a vendored, slightly adapted copy of `CLD2`.  This is the same Google-built language detection library used in Chrome.
* Text tokenization, stemming and stopword detection in many languages.
* A comfortable, C++11 HTML DOM interface backed by Google's `libgumbo-parser`.
* A variety of hash functions stolen from Google's `smhasher`, plus Bloom Filter and Simhash implementations built on top of these.


## Licensing

### score itself
All original score code is under the MIT license.  This includes all code that is not under a `vendored/` subdirectory in the source tree.

Vendored libraries are under several different licenses, as indicated in the next section.  In general, third-party libraries under commercial-friendly MIT/BSD-style licenses have been preferred wherever possible.  There is currently a single exception, Mozilla's `libcharsetdetect`, which is LGPL-licensed.

If you are developing commercial software *which you are planning to actually distribute to third parties*, the presence of LGPL code will be restrictive and you will need to find a replacement for `libcharsetdetect`.  If you are developing SaaS-style server-side applications, the presence of LGPL code is not particularly restrictive but may run counter to your organization's internal guidelines.

score does not and will never contain any AGPL-licensed code, due to the restrictions it places on even in-house proprietary applications.

### vendored third-party code
* CLD2: [Apache](licensing/third_party/CLD2)
* libcharsetdetect: [lgpl](licensing/third_party/libcharsetdetect)
* libgumbo-parser: [Apache](licensing/third_party/libgumbo-parser)
* smhasher: public domain
* utfcpp: MIT
* pugixml: MIT
* seastar (misc utility classes): Apache 2.0
* folly (misc utility classes): Apache 2.0
