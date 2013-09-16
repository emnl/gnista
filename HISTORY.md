# Version 0.0.4

Release date 2013-09-16

### Added

* History for minor changes.
* Lesson learned regarding "gem yank".

# Version 0.0.3

Release date 2013-09-16

### Added

* ```logpath``` for ```Logwriter```, ```Logreader``` and ```Hash```.
* ```hashpath``` for ```Hash```.
* Shorthand syntax for puts and gets. Similar to Ruby's native hash: []= and [].
* ```empty?``` method for ```Hash```.
* ```keys``` and ```values``` methods for ```Hash```.
* ```has_key?``` method for ```Hash```.
* ```maxkeylen``` and ```maxvaluelen``` for ```Logreader```.
* ```Logreader``` and ```Hash``` mixin [Enumerable](http://ruby-doc.org/core-2.0.0/Enumerable.html).
* ```write_batch``` implemented for ```Logwriter```.
* Useful ```inspect``` for all classes.

### Fixed

* String-check for all input to ```Logwriter``` and ```Hash```.

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