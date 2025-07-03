# Noise NOW!

Too many bad sounds around you? Noise NOW! provides the quickest way to get some soothing random noise going by simply typing in `noisenow` into the console. It comes in white, pink and brown varieties alongside a few configuration options, but the defaults are a loop of 10 seconds of looping 44.1 kHz stereo white noise. Configuration options can be shown by running `noisenow -h`.

## Building

The only dependency of this project is OpenAL. Simply run these commands to get a build of Noise NOW!:

```
git clone https://github.com/BlueStaggo/noisenow && cd noisenow
mkdir build && cd build
cmake .. -G Ninja
ninja
```
