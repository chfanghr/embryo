# Embryo: lua compiler and vm, in C++17.

[![works badge](https://cdn.jsdelivr.net/gh/nikku/works-on-my-machine@v0.2.0/badge.svg)](https://github.com/nikku/works-on-my-machine)

## For those who want to play around with the project

* This project will run on Linux and macOS only.
* Install [`nix`](https://nixos.org/download.html#download-nix) and [enable flake support](https://nixos.wiki/wiki/Flakes).
* Clone the project and `cd` into the directory.
* Run `nix develop`. Wait for a moment and you will be dropped into a shell, with all the tools you need in the `PATH`.
* Use `treefmt` to format the project. (WIP: add clang format settings).
* Any contribution is welcome!

By the way, If you have [`direnv`](https://direnv.net/) and (optionally) [`nix-direnv`](https://github.com/nix-community/nix-direnv) installed, run the following commands at the *root* of the project so that you will have everything setup whenever you enter the project:

```sh
echo "use flake" >> .envrc; direnv allow
```

## License

MIT, see [LICENSE](LICENSE).
