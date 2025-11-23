# MLP Pixi Designer Prototype

Minimal high-performance WebGL prototype using PixiJS.

## Features Implemented
- Electron shell with PixiJS Application
- 100 sample box widgets rendered via `PIXI.Graphics`
- Drag interaction enabled for the first box (pointerdown/move/up)
- Lightweight FPS meter for performance verification

## Run
```bash
cd editors/mlp_pixi_designer
npm install
npm start
```

## Next Steps
- Generalize drag behavior for all widgets
- Introduce unified Core Domain Model adapter layer
- Add selection + transformer visuals
- Integrate Monaco for MLP code view
