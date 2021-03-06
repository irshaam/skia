importScripts('https://particles.skia.org/dist/canvaskit.js');
importScripts('shared.js');

const transferCanvasToOffscreenPromise =
    new Promise((resolve) => addEventListener('message', resolve));
const canvasKitInitPromise =
    CanvasKitInit({locateFile: (file) => 'https://particles.skia.org/dist/'+file});
const skottieJsonPromise =
    fetch('https://storage.googleapis.com/skia-cdn/misc/lego_loader.json')
    .then((response) => response.text());

Promise.all([
    transferCanvasToOffscreenPromise,
    canvasKitInitPromise,
    skottieJsonPromise
]).then(([
    { data: { offscreenCanvas } },
    CanvasKit,
    jsonStr
]) => {
    const surface = CanvasKit.MakeWebGLCanvasSurface(offscreenCanvas, null);
    if (!surface) {
        throw 'Could not make canvas surface';
    }

    SkottieExample(CanvasKit, surface, jsonStr);
});
