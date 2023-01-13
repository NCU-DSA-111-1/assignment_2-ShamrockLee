# myshogi

myshogi is a Shogi program that allows two players to play Shogi inside the command line on one computer.

It draws a red- and blue- Kanji board in the beginning of each player's turn. The color blue is for the first (black) player, and the color red for the second (white) player. A linked list is implemented to store the steps and to write out to a file.

It uses GNU Shogi as its backend by calling the `gnushogi` executable in another process, redirecting the stdin, stdout and stderr, and input / extract / translate the text streams programmatically.

The assignment requires us to use libev to implement the timing for each player. Due to the time limitation, the libev part hasn't been implemented and the built-in timing from GNU Shogi is displayed.

The main executable, `myshogi` currently suffers from bugs related to the text flows. Partial effort can still be seen from the `tranlate-boald` test. See the Test section for detail.

## Build from source

### Dependencies

The following dependencies needs to be setup before building with Meson.

*   Development dependencies
    *   Meson or Muon
    *   Ninja, Samurai, or other Meson backends
    *   Pkg-Config
*   Run-time dependencies
    *   [GNU Shogi](https://www.gnu.org/software/gnushogi/)
    *   [libev](http://software.schmorp.de/pkg/libev.html)

### Build with Meson

This project uses Meson as a packaging-independent configuration generator.
It supports several builders ("backends") such as Ninja, but Make is not supported by design.

Meson generates `build.ninja` for Ninja just like GNU Automake or CMake generates `Makefile` for GNU Make.

Build as

```sh
meson setup build.tmp
cd build.tmp
meson compile
```

The executable `myshogi` can be found inside `build.tmp/src/myshogi`.

### Nix-based development environment

This project uses Nix as a project manager.
To enter a shell with the build environment (and all the dependencies) set up, run

Even if `nix` is not available, you can use the docker image instead:

```
docker run -ti --rm -v /tmp:/tmp -v "$PWD":"$PWD" -w "$PWD" nixos/nix:2.12.0 nix --extra-experimental-features "nix-command flakes" develop
```

After that, you'll find yourself inside a shell session with all the dependencies available to compile the projects.

## Tests

An executable `translate-board` is generated to test the funtionality of the `translate_board` library. After compiling the project with Meson, it will be available at `build.tmp/src/translate-board`.

Run the following commands to test it directly:

```console
$ cat <<EOF | build.tmp/src/translate-board

Black 1:28  White 1:01

 - - P+s - -+r K L
 - - - g - - - - -
 - - - - - G L S+b
 P - - - P P - - -
 p - - - - - P P p
 - - s - g - - - -
 - p - p k p+N - -
 - R - - - - - p -
 l - - - - - g - l

black 4P 3N 1S 1B
white 2P
EOF
translate_board called

Time left: Black 1:28, White 1:01

    步全    竜王香
      金
          金香銀馬
步      步步
步          步步步
    銀  金
  步  步王步圭
  飛          步
香          金  香

black 4步 3桂 1銀 1角
white 2步
```

Or test it with `gnushogi`

```console
$ gnushogi | build.tmp/src/translate-board 1
GNU Shogi 1.4.2
translate_board called
bd

Time left: Black 5:00, White 5:00

香桂銀金王金銀桂香
  飛          角  
步步步步步步步步步
                  
                  
                  
步步步步步步步步步
  角          飛  
香桂銀金王金銀桂香

black 
white 
^C

```

## What I learned from this assignment

Aside from the linked-list, escape strings (to add the color), process and io redirection knowledge and techniques,
the experience to develop this project also suggests
1.  the importance to choose the right tooling ("Don't script with C!"), and
2.  the cost of integration to a third-party project (like GNU Shogi) sometimes out-weights the cost to implement one from scratch.
