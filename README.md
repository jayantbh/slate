# Slate ![Build Status](https://travis-ci.org/jayantbh/slate.svg?branch=master)
This project aims to build a _proof-of-concept_ command line text editor.

#### Collaborators
- Sagnik Chatterjee (@chatsagnik)
- Jayant Bhawal (@jayantbh)
- Soumyarup Paul (@soumyarup-paul)
- Subham Singh (@ssubham-singh)

#### Instructions
Build using `cmake . && make`.  
Run `./backend.out` for testing the backend.  
Run `./slate.out` to test the editor.  
Quick run:
```
cmake . && make && ./slate.out file.txt
```

#### Pre-requisites
- CMake 2.5.1 or above (optional)
- Make (optional)
- GCC
- NCurses library
- Panel library

#### Project Updates
- Supports _insert_, _delete_, _cursor movement_, _per-line scrolling_, _undo_, _find_, _replace_, and _persistence_.
- Left to do: _selection_, `copy()`, and `paste()`.
