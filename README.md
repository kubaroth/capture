# Capture
X11 screen capture command line tool. 
Generated image is converted to a pdf file.

## Build
```
git clone --recursive https://github.com/kubaroth/capture.git
mkdir __build
cd __build
cmake ..
make
```
## Use
Entire screen capture:
```
./capture -cs
```
Preview the largest segment to capture from captured screen
```
./capture -cs -preview-segments
```
Capture specific window:
```
./capture -cw Chromium
```

Preview the largest segment to capture from specified window:
```
./capture -cw Chromium -preview-segments
```

## TODO
- skip x11 and add option to apply crop and generated pdf to an input png file
- add png loader
- add tests
- convert to a single pdf using pdfWrite
- enable different formats

## Dependencies
- X11
- PDF-Writer (https://github.com/galkahana/PDF-Writer.git)

