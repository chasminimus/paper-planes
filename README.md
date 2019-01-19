# Readme

![Paper Planes](https://i.imgur.com/LpOuvEF.png)

## Dependencies
- ofxAssimpModelLoader
- ofxDatGui

## Usage

Make sure the model is in the `bin/data/model` directory and the required fonts for ofxDatGui are in the `bin/data` directory as well.

The main component of the project is the `Flock` class. Instantiate one, call the `init()` method  and focus a camera on it. In `ofApp::draw()`, call the flock's  `customDraw()` method.
Make sure to call `ofDisableDepthTest()` at some point after drawing the flock or the GUI won't render properly.
