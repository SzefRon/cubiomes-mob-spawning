# cubiomes-mob-spawning

Forked from [cubiomes](https://github.com/Cubitect/cubiomes), a project meant to simulate Minecraft's mob spawning that happens on chunk generation.

In recent versions mob spawning finalization also became seed dependant, which means processes like giving foxes random items when they generate with the world now also depends on the seed.

Special thanks to [Kris](https://github.com/Kludwisz) for reconstructing the random device used for mob spawning finalization!

## Compiling

Same requirements as [cubiomes](https://github.com/Cubitect/cubiomes).

Do
```
make release
```
to build to `.\build.exe`