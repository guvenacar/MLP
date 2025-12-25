import * as PIXI from 'pixi.js';
import { initPixi, attachFps } from './pixiApp.js';
import { createSampleWidgets } from './model.js';

// Selection state
let selectedId = null;

function renderWidgets(app, widgets) {
  widgets.forEach((w) => {
    const g = new PIXI.Graphics();
    drawBox(g, w);
    g.x = w.geom.x;
    g.y = w.geom.y;
    g.eventMode = 'static';
    g.cursor = 'pointer';

    g.on('pointerdown', (e) => {
      // Left button only
      if (e.button === 0) {
        selectWidget(app, g, w);
        startDrag(g, w, e);
      }
    });
    g.on('pointerup', () => stopDrag(g));
    g.on('pointerupoutside', () => stopDrag(g));
    g.on('pointermove', (e) => dragMove(g, w, e));

    app.stage.addChild(g);
  });
}

function drawBox(g, w) {
  g.clear();
  // Use legacy drawing API for broader Pixi version compatibility
  g.lineStyle(1, w.state.selected ? 0xffcc00 : w.style.stroke);
  g.beginFill(w.style.fill);
  g.drawRect(0, 0, w.geom.width, w.geom.height);
  g.endFill();
  // label
  if (!g.labelText) {
    g.labelText = new PIXI.Text({
      text: w.id,
      style: { fontSize: 10, fill: 0xffffff, fontFamily: 'Arial' }
    });
    g.labelText.x = 4; g.labelText.y = 4;
    g.addChild(g.labelText);
  } else {
    g.labelText.text = w.id;
  }
}

function selectWidget(app, g, w) {
  // Deselect previous
  app.stage.children.forEach(ch => {
    if (ch !== g && ch.widgetModel) {
      ch.widgetModel.state.selected = false;
      drawBox(ch, ch.widgetModel);
    }
  });
  w.state.selected = true;
  selectedId = w.id;
  drawBox(g, w);
}

function startDrag(g, w, e) {
  g.dragging = true;
  const pos = e.global;
  g.dragOffsetX = pos.x - g.x;
  g.dragOffsetY = pos.y - g.y;
  g.widgetModel = w;
}
function stopDrag(g) { g.dragging = false; }
function dragMove(g, w, e) {
  if (!g.dragging) return;
  const pos = e.global;
  g.x = pos.x - g.dragOffsetX;
  g.y = pos.y - g.dragOffsetY;
  w.geom.x = g.x; w.geom.y = g.y;
}

// Boot
const root = document.getElementById('app');
const app = initPixi(root);
const widgets = createSampleWidgets(100);
renderWidgets(app, widgets);
attachFps(app, document.getElementById('fps'));

window.addEventListener('resize', () => {
  app.renderer.resize(window.innerWidth, window.innerHeight);
});
