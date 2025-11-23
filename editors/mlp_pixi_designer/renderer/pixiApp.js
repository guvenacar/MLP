import * as PIXI from 'pixi.js';

export function initPixi(rootEl) {
  const app = new PIXI.Application({
    background: '#1e1e1e',
    antialias: true,
    resolution: window.devicePixelRatio,
    autoDensity: true,
  });
  rootEl.appendChild(app.view);
  app.view.style.width = '100%';
  app.view.style.height = '100%';
  return app;
}

export function attachFps(app, el) {
  let lastTime = performance.now();
  let frames = 0;
  app.ticker.add(() => {
    frames++;
    const now = performance.now();
    if (now - lastTime >= 1000) {
      el.textContent = `FPS: ${frames}`;
      frames = 0;
      lastTime = now;
    }
  });
}
