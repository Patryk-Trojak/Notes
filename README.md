# Notes
It is a minimalistic note-taking app that allows you to organize your notes using folders

## Screenshots
![Alt text](/screenshots/NotesScreenshot.png?raw=true)
![Alt text](/screenshots/NotesEditingScreenshot.png?raw=true)

## Technologies Used
- C++17
- QT 6.5.0

## Compiling
You need:
- QT 6
- C++17 compliant compiler
- CMake build system

You can run cmake to generate build system which then you can use to compile the app.
For e.g., on Linux/OS X/FreeBSD:
```
$ git clone https://github.com/Patryk-Trojak/Notes.git
$ cd Notes
$ mkdir build/ && cd build/
$ cmake ../
$ make -j4    #Replace 4 with however many cores you want to use
```