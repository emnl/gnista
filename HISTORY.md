# Version 0.0.2

Release date 2013-09-12

### Added

* ```each``` in logreader now provides a __type__ parameter containing either :put or :delete.

### Fixed

* Fix memory leak on ```each/get``` failure.
* Two allocations per call instead of one each entry in ```each```.
* ```each``` in logreader returns nil instead of empty string.


# Version 0.0.1

Release date 2013-09-11

__Initial release__